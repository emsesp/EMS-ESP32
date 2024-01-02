#include "async_worker.h"

bool is_on_async_worker_thread(void)
{
    // is our handle one of the known async handles?
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    for (int i = 0; i < ASYNC_WORKER_COUNT; i++) {
        if (worker_handles[i] == handle) {
            return true;
        }
    }
    return false;
}

// Submit an HTTP req to the async worker queue
esp_err_t submit_async_req(httpd_req_t *req, httpd_req_handler_t handler)
{
    // must create a copy of the request that we own
    httpd_req_t* copy = NULL;
    esp_err_t err = httpd_req_async_handler_begin(req, &copy);
    if (err != ESP_OK) {
        return err;
    }

    httpd_async_req_t async_req = {
        .req = copy,
        .handler = handler,
    };

    // How should we handle resource exhaustion?
    // In this example, we immediately respond with an
    // http error if no workers are available.
    int ticks = 0;

    // counting semaphore: if success, we know 1 or
    // more asyncReqTaskWorkers are available.
    if (xSemaphoreTake(worker_ready_count, ticks) == false) {
        ESP_LOGE(PH_TAG, "No workers are available");
        httpd_req_async_handler_complete(copy); // cleanup
        return ESP_FAIL;
    }

    // Since worker_ready_count > 0 the queue should already have space.
    // But lets wait up to 100ms just to be safe.
    if (xQueueSend(async_req_queue, &async_req, pdMS_TO_TICKS(100)) == false) {
        ESP_LOGE(PH_TAG, "worker queue is full");
        httpd_req_async_handler_complete(copy); // cleanup
        return ESP_FAIL;
    }

    return ESP_OK;
}

void async_req_worker_task(void *p)
{
    ESP_LOGI(PH_TAG, "starting async req task worker");

    while (true) {

        // counting semaphore - this signals that a worker
        // is ready to accept work
        xSemaphoreGive(worker_ready_count);

        // wait for a request
        httpd_async_req_t async_req;
        if (xQueueReceive(async_req_queue, &async_req, portMAX_DELAY)) {

            ESP_LOGI(PH_TAG, "invoking %s", async_req.req->uri);

            // call the handler
            async_req.handler(async_req.req);

            // Inform the server that it can purge the socket used for
            // this request, if needed.
            if (httpd_req_async_handler_complete(async_req.req) != ESP_OK) {
                ESP_LOGE(PH_TAG, "failed to complete async req");
            }
        }
    }

    ESP_LOGW(PH_TAG, "worker stopped");
    vTaskDelete(NULL);
}

void start_async_req_workers(void)
{

    // counting semaphore keeps track of available workers
    worker_ready_count = xSemaphoreCreateCounting(
        ASYNC_WORKER_COUNT,  // Max Count
        0); // Initial Count
    if (worker_ready_count == NULL) {
        ESP_LOGE(PH_TAG, "Failed to create workers counting Semaphore");
        return;
    }

    // create queue
    async_req_queue = xQueueCreate(1, sizeof(httpd_async_req_t));
    if (async_req_queue == NULL){
        ESP_LOGE(PH_TAG, "Failed to create async_req_queue");
        vSemaphoreDelete(worker_ready_count);
        return;
    }

    // start worker tasks
    for (int i = 0; i < ASYNC_WORKER_COUNT; i++) {

        bool success = xTaskCreate(async_req_worker_task, "async_req_worker",
                                    ASYNC_WORKER_TASK_STACK_SIZE, // stack size
                                    (void *)0, // argument
                                    ASYNC_WORKER_TASK_PRIORITY, // priority
                                    &worker_handles[i]);

        if (!success) {
            ESP_LOGE(PH_TAG, "Failed to start asyncReqWorker");
            continue;
        }
    }
}

/****
 * 
 * This code is backported from the 5.1.x branch
 * 
****/

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* Calculate the maximum size needed for the scratch buffer */
#define HTTPD_SCRATCH_BUF  MAX(HTTPD_MAX_REQ_HDR_LEN, HTTPD_MAX_URI_LEN)

/**
 * @brief   Auxiliary data structure for use during reception and processing
 *          of requests and temporarily keeping responses
 */
struct httpd_req_aux {
    struct sock_db *sd;                             /*!< Pointer to socket database */
    char            scratch[HTTPD_SCRATCH_BUF + 1]; /*!< Temporary buffer for our operations (1 byte extra for null termination) */
    size_t          remaining_len;                  /*!< Amount of data remaining to be fetched */
    char           *status;                         /*!< HTTP response's status code */
    char           *content_type;                   /*!< HTTP response's content type */
    bool            first_chunk_sent;               /*!< Used to indicate if first chunk sent */
    unsigned        req_hdrs_count;                 /*!< Count of total headers in request packet */
    unsigned        resp_hdrs_count;                /*!< Count of additional headers in response packet */
    struct resp_hdr {
        const char *field;
        const char *value;
    } *resp_hdrs;                                   /*!< Additional headers in response packet */
    struct http_parser_url url_parse_res;           /*!< URL parsing result, used for retrieving URL elements */
#ifdef CONFIG_HTTPD_WS_SUPPORT
    bool ws_handshake_detect;                       /*!< WebSocket handshake detection flag */
    httpd_ws_type_t ws_type;                        /*!< WebSocket frame type */
    bool ws_final;                                  /*!< WebSocket FIN bit (final frame or not) */
    uint8_t mask_key[4];                            /*!< WebSocket mask key for this payload */
#endif
};

esp_err_t httpd_req_async_handler_begin(httpd_req_t *r, httpd_req_t **out)
{
    if (r == NULL || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // alloc async req
    httpd_req_t *async = (httpd_req_t *)malloc(sizeof(httpd_req_t));
    if (async == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(async, r, sizeof(httpd_req_t));

    // alloc async aux
    async->aux = (httpd_req_aux *)malloc(sizeof(struct httpd_req_aux));
    if (async->aux == NULL) {
        free(async);
        return ESP_ERR_NO_MEM;
    }
    memcpy(async->aux, r->aux, sizeof(struct httpd_req_aux));

    // not available in 4.4.x
    // mark socket as "in use"
    // struct httpd_req_aux *ra = r->aux;
    //ra->sd->for_async_req = true; 

    *out = async;

    return ESP_OK;
}

esp_err_t httpd_req_async_handler_complete(httpd_req_t *r)
{
    if (r == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // not available in 4.4.x
    // struct httpd_req_aux *ra = (httpd_req_aux *)r->aux;
    // ra->sd->for_async_req = false;

    free(r->aux);
    free(r);

    return ESP_OK;
}