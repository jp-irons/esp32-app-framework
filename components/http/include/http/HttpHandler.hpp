// components/http/include/http/HttpHandler.hpp
#pragma once
#include "common/Result.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

namespace http {

class HttpHandler {
public:
    virtual ~HttpHandler() = default;

    virtual common::Result handle(HttpRequest& req, HttpResponse& res) = 0;
};

} // namespace http