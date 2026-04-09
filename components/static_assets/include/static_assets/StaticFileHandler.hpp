#pragma once

#include "http/HttpHandler.hpp"
#include "static_assets/EmbeddedAssetTable.hpp"

#include <string>

namespace static_assets {

class StaticFileHandler : public http::HttpHandler {
  public:
    StaticFileHandler(std::string basePath, std::string defaultFile);

    common::Result handle(http::HttpRequest &request, http::HttpResponse &response) override;

  private:
    std::string base;
    std::string defaultFile;
    EmbeddedAssetTable table; // owns its own table

    std::string resolvePath(std::string_view uri) const;
    static const char *contentTypeForPath(const std::string &path);
};

} // namespace static_assets