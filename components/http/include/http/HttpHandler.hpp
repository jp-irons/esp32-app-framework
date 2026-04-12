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
	
	static std::string extractAction(const char *uri) {
	    std::string path(uri);
	    auto pos = path.find_last_of('/');
	    if (pos == std::string::npos || pos == path.length() - 1) {
	        return {}; // no action found
	    }
	    return path.substr(pos + 1);
	}

	
};

} // namespace http