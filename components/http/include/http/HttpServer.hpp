#pragma once

#include <string>
#include "esp_http_server.h"
#include "http/HttpHandler.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

namespace http {

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    void start();
    void stop();

    void addRoute(const std::string& path, HttpHandler* handler);

private:
    httpd_handle_t server;

    static esp_err_t handlerThunk(httpd_req_t* req);
};

} // namespace http

