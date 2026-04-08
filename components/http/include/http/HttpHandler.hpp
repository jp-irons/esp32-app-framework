// components/http/include/http/HttpHandler.hpp
#pragma once

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

namespace http {

class HttpHandler {
public:
    virtual ~HttpHandler() = default;

    virtual bool handle(HttpRequest& req, HttpResponse& res) = 0;
};

} // namespace http