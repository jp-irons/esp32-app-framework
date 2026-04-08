#pragma once

#include "http/HttpHandler.hpp"
namespace http {
class HttpRequest;
class HttpResponse;
} // namespace http

namespace credential_store {

class CredentialStore;

class CredentialApiHandler : public http::HttpHandler {
  public:
    CredentialApiHandler(credential_store::CredentialStore &store);
    bool handle(http::HttpRequest &req, http::HttpResponse &res) override;

  private:
    bool handleList(http::HttpResponse &res);
    bool handleSubmit(const http::HttpRequest &req, http::HttpResponse &res);
    bool handleDelete(const http::HttpRequest &req, http::HttpResponse &res);
    bool handleClear(http::HttpResponse &res);
	std::string extractAction(const char* uri);

    credential_store::CredentialStore &store;
};

} // namespace credential_store