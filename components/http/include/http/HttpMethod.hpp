#pragma once

#include <string>
namespace http {

enum class HttpMethod {
    Get,
    Post,
    Put,
    Delete,
    Patch,
    Head,
    Options
};

inline std::string toString(HttpMethod method) {
    switch (method) {
        case HttpMethod::Get:     return "Get";
        case HttpMethod::Post:    return "Post";
        case HttpMethod::Put:     return "Put";
        case HttpMethod::Delete:  return "Delete";
        case HttpMethod::Patch:   return "Patch";
        case HttpMethod::Head:    return "Head";
        case HttpMethod::Options: return "Options";
    }
    return "Unknown";
}

}