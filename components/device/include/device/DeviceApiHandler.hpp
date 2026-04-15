#pragma once

#include "http/HttpHandler.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "common/Result.hpp"

namespace device {
	
class DeviceApiHandler : public http::HttpHandler {
public:
    explicit DeviceApiHandler();
    virtual ~DeviceApiHandler() = default;

    common::Result handle(http::HttpRequest& req, http::HttpResponse& res) override;

private:
    common::Result handleReboot(http::HttpRequest& req, http::HttpResponse& res);
};
} // namespace