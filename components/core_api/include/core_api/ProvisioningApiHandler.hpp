#pragma once

class ProvisioningStateMachine;

namespace http {
    class HttpRequest;
    class HttpResponse;
}

namespace core_api {

class ProvisioningApiHandler {
public:
    explicit ProvisioningApiHandler(ProvisioningStateMachine& provisioning);

    bool handle(const http::HttpRequest& req, http::HttpResponse& res);

private:
    void handleStatus(http::HttpResponse& res);
    void handleStart(http::HttpResponse& res);
    void handleComplete(http::HttpResponse& res);

    ProvisioningStateMachine& provisioning;
};

} // namespace core_api