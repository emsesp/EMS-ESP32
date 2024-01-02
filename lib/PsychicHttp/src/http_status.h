#ifndef MICRO_HTTP_STATUS_H
#define MICRO_HTTP_STATUS_H

#include <stdbool.h>

bool http_informational(int code);
bool http_success(int code);
bool http_redirection(int code);
bool http_client_error(int code);
bool http_server_error(int code);
bool http_failure(int code);
const char *http_status_group(int code);
const char *http_status_reason(int code);

#endif // MICRO_HTTP_STATUS_H