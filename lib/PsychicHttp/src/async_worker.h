#ifndef async_worker_h
#define async_worker_h

#include "PsychicCore.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define ASYNC_WORKER_TASK_PRIORITY      5
#define ASYNC_WORKER_TASK_STACK_SIZE    (4*1024)
#define ASYNC_WORKER_COUNT 8

// Async requests are queued here while they wait to be processed by the workers
static QueueHandle_t async_req_queue;

// Track the number of free workers at any given time
static SemaphoreHandle_t worker_ready_count;

// Each worker has its own thread
static TaskHandle_t worker_handles[ASYNC_WORKER_COUNT];

typedef esp_err_t (*httpd_req_handler_t)(httpd_req_t *req);

typedef struct {
    httpd_req_t* req;
    httpd_req_handler_t handler;
} httpd_async_req_t;

bool is_on_async_worker_thread(void);
esp_err_t submit_async_req(httpd_req_t *req, httpd_req_handler_t handler);
void async_req_worker_task(void *p);
void start_async_req_workers(void);

esp_err_t httpd_req_async_handler_begin(httpd_req_t *r, httpd_req_t **out);
esp_err_t httpd_req_async_handler_complete(httpd_req_t *r);

#endif //async_worker_h