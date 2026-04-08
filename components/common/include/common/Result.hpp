#pragma once

namespace common {

enum class Result {
    Ok,            // Operation succeeded
    NotFound,      // Resource not found
    BadRequest,    // Invalid input or malformed request
    Forbidden,     // Access denied
    InternalError, // Unexpected failure
    Unsupported    // Operation not supported
};

}
