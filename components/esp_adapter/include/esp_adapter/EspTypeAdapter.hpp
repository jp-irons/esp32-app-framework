#pragma once
#include "common/Result.hpp"
#include "esp_err.h"
#include "esp_http_server.h"
#include "http/HttpMethod.hpp"

namespace esp_adapter {
inline esp_err_t toEspError(common::Result r) {
    using common::Result;

    switch (r) {
        case Result::Ok:
            return ESP_OK;
        case Result::NotFound:
            return ESP_ERR_NOT_FOUND;
        case Result::BadRequest:
            return ESP_ERR_INVALID_ARG;
        case Result::Forbidden:
            return ESP_ERR_INVALID_STATE;
        case Result::InternalError:
            return ESP_FAIL;
        case Result::Unsupported:
            return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_FAIL;
}

inline common::Result toResult(esp_err_t err) {
    using common::Result;

    switch (err) {
        case ESP_OK:
            return Result::Ok;

        case ESP_ERR_NOT_FOUND:
            return Result::NotFound;

        case ESP_ERR_INVALID_ARG:
            return Result::BadRequest;

        case ESP_ERR_INVALID_STATE:
            return Result::Forbidden;

        case ESP_ERR_NOT_SUPPORTED:
            return Result::Unsupported;

        case ESP_FAIL:
        default:
            return Result::InternalError;
    }
}

inline httpd_method_t toEspIdfMethod(http::HttpMethod method) {
    switch (method) {
        case http::HttpMethod::Get:
            return HTTP_GET;
        case http::HttpMethod::Post:
            return HTTP_POST;
        case http::HttpMethod::Put:
            return HTTP_PUT;
        case http::HttpMethod::Delete:
            return HTTP_DELETE;
        case http::HttpMethod::Patch:
            return HTTP_PATCH;
        case http::HttpMethod::Head:
            return HTTP_HEAD;
        case http::HttpMethod::Options:
            return HTTP_OPTIONS;
    }
    return HTTP_GET;
}

} // namespace esp_adapter