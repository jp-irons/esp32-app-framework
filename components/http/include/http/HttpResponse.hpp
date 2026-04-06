#pragma once
#include "esp_http_server.h"

#include <string>
#include <string_view>

// TODO move method defns back from header to cpp
namespace http {

class HttpResponse {
  public:
    explicit HttpResponse(httpd_req *r)
        : req(r) {}

    void send(const unsigned char *data, unsigned int size) {
        httpd_resp_send(req, reinterpret_cast<const char *>(data), size);
    }

    void send(std::string_view data) {
        httpd_resp_send(req, data.data(), data.size());
    }

	void setType(const char* type) {
	    httpd_resp_set_type(req, type);
	}


   // -----------------------------
    // Convenience helpers
    // -----------------------------
	void text(const std::string& body) {
	    httpd_resp_set_type(req, "text/plain");
	    httpd_resp_send(req, body.c_str(), body.size());
	}

	void json(const std::string& body) {
	    httpd_resp_set_type(req, "application/json");
	    httpd_resp_send(req, body.c_str(), body.size());
	}

	void jsonStatus(const char* status) {
	    std::string body = std::string("{\"status\":\"") + status + "\"}";
	    json(body);
	}

  private:
    httpd_req *req;
};

} // namespace http