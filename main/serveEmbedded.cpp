#include "serveEmbedded.hpp"
#include "EmbeddedFiles.hpp"

#include "esp_log.h"

#include <string>

static const char* TAG = "serveEmbedded";

esp_err_t serveEmbedded(httpd_req_t* req, const char* basePath)
{
    ESP_LOGD(TAG, "URI to serve: %s", req->uri);

    std::string path = basePath;

    // Ensure basePath ends with '/'
    if (!path.empty() && path.back() != '/')
        path.push_back('/');

    // Append the request URI (skipping leading '/')
    const char* uri = req->uri;
    if (uri[0] == '/')
        uri++;

    path += uri;

    // If the resulting path ends with '/', serve index.html
    if (!path.empty() && path.back() == '/')
        path += "index.html";

    ESP_LOGD(TAG, "Full static file path: %s", path.c_str());

    // Lookup embedded file
    const EmbeddedFile* file = getEmbeddedFile(path);
    if (!file) {
        ESP_LOGW(TAG, "NOT FOUND for path: %s", path.c_str());
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not found");
        return ESP_OK;
    }

    // Set MIME type
    httpd_resp_set_type(req, file->mime);

    // Send 
    return httpd_resp_send(req,
                reinterpret_cast<const char*>(file->start),
                file->size);

}