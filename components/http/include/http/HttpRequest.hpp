#pragma once

#include "esp_http_server.h"
#include <string>
#include <string_view>

namespace http {

// TODO move method defns back from header to cpp
class HttpRequest {
  public:
    explicit HttpRequest(httpd_req *r)
        : req(r) {}

    const char *path() const {
        return req->uri;
    }

    std::string body() const;  // keep this out-of-line

    std::string_view uri() const {
        return req->uri;
    }

    httpd_req *raw() const {
        return req;
    }

private:
    httpd_req *req;
};

} // namespace http