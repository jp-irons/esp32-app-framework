#pragma once
#include "common/Result.hpp"
#include "esp_adapter/EspTypeAdapter.hpp"
#include "esp_http_server.h"

#include <string>
#include <string_view>

// TODO move method defns back from header to cpp
namespace http {

using namespace common;

class HttpResponse {
  public:
    explicit HttpResponse(httpd_req *r)
        : req(r) {}

    Result send(const unsigned char *data, unsigned int size) {
        esp_err_t err = httpd_resp_send(req, reinterpret_cast<const char *>(data), size);
        return esp_adapter::toResult(err);
    }

    Result send(std::string_view data) {
        esp_err_t err = httpd_resp_send(req, data.data(), data.size());
        return esp_adapter::toResult(err);
    }

    void setType(const char *type) {
        httpd_resp_set_type(req, type);
    }

	Result notFound(std::string_view data) {
	    httpd_resp_set_status(req, "404 Not Found");
	    httpd_resp_set_type(req, "text/plain");
	    esp_err_t err = httpd_resp_send(req, data.data(), data.size());
	    return esp_adapter::toResult(err);
	}

    // -----------------------------
    // Convenience helpers
    // -----------------------------
    // TODO rename these to sendText etc.
    Result text(const std::string &body) {
        httpd_resp_set_type(req, "text/plain");
        esp_err_t err = httpd_resp_send(req, body.c_str(), body.size());
        return esp_adapter::toResult(err);
    }

    Result json(const std::string &body) {
        httpd_resp_set_type(req, "application/json");
        esp_err_t err = httpd_resp_send(req, body.c_str(), body.size());
        return esp_adapter::toResult(err);
    }

    Result jsonStatus(const char *status) {
        std::string body = std::string("{\"status\":\"") + status + "\"}";
        return json(body);
    }

  private:
    httpd_req *req;
};

} // namespace http