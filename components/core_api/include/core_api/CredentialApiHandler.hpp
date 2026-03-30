#pragma once

class CredentialStore;
class ProvisioningStateMachine;

namespace http {
    class HttpRequest;
    class HttpResponse;
}

namespace core_api {

class CredentialApiHandler {
public:
    CredentialApiHandler(CredentialStore& store,
                         ProvisioningStateMachine& provisioning);

    bool handle(const http::HttpRequest& req, http::HttpResponse& res);

private:
    void handleList(http::HttpResponse& res);
    void handleSubmit(const http::HttpRequest& req, http::HttpResponse& res);
    void handleDelete(const http::HttpRequest& req, http::HttpResponse& res);
    void handleClear(http::HttpResponse& res);

    CredentialStore& store;
    ProvisioningStateMachine& provisioning;
};

} // namespace core_api