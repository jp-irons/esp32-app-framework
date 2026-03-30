#pragma once

#include "credential_store/CredentialStore.hpp"
#include "wifi_manager/ProvisioningStateMachine.hpp"
class CredentialStore;
class ProvisioningStateMachine;

namespace http {
    class HttpRequest;
    class HttpResponse;
}

namespace core_api {

class CredentialApiHandler {
public:
    CredentialApiHandler(credential_store::CredentialStore& store,
                         wifi_manager::ProvisioningStateMachine& provisioning);

    bool handle(const http::HttpRequest& req, http::HttpResponse& res);

private:
    void handleList(http::HttpResponse& res);
    void handleSubmit(const http::HttpRequest& req, http::HttpResponse& res);
    void handleDelete(const http::HttpRequest& req, http::HttpResponse& res);
    void handleClear(http::HttpResponse& res);

    credential_store::CredentialStore& store;
    wifi_manager::ProvisioningStateMachine& provisioning;
};

} // namespace core_api