#include "http/HttpServer.hpp"

namespace http {

HttpServer::HttpServer() {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    if (server)
        return;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);
}

void HttpServer::stop() {
    if (server) {
        httpd_stop(server);
        server = nullptr;
    }
}

void HttpServer::addRoute(const std::string &path, HttpHandler *handler) {
    httpd_uri_t uri = {
        .uri = path.c_str(), .method = HTTP_GET, .handler = &HttpServer::handlerThunk, .user_ctx = handler};

    httpd_register_uri_handler(server, &uri);
}

esp_err_t HttpServer::handlerThunk(httpd_req_t *req) {
    auto *handler = static_cast<http::HttpHandler *>(req->user_ctx);
    http::HttpRequest request(req);
    http::HttpResponse response(req);
    handler->handle(request, response);
    return ESP_OK;
}

} // namespace http