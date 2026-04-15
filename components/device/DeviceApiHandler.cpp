#include "device/DeviceApiHandler.hpp"
#include "logger/Logger.hpp"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace device {
	
static logger::Logger log{"DeviceApiHandler"};

DeviceApiHandler::DeviceApiHandler() {
	log.debug("contructor");
}

using namespace common;
Result DeviceApiHandler::handle(http::HttpRequest& req, http::HttpResponse& res)
{
	log.debug("handle");
    const std::string action = http::HttpHandler::extractAction(req.path());

    if (action == "reboot") {
        return handleReboot(req, res);
    }

//    // Unknown action
//    res.setStatus(404);
//    res.setContentType("application/json");
//    res.send("{\"error\":\"unknown action\"}");
    return common::Result::Unsupported;
}

common::Result DeviceApiHandler::handleReboot(http::HttpRequest& req, http::HttpResponse& res)
{
	log.debug("handleReboot");
	return common::Result::Unsupported;
//    // Only allow POST
//    if (req.method() != http::Method::POST) {
//        res.setStatus(405);
//        res.setContentType("application/json");
//        res.send("{\"error\":\"method not allowed\"}");
//        return common::Result::Error("Invalid method");
//    }
//
//    // Respond BEFORE rebooting
//    res.setStatus(200);
//    res.setContentType("application/json");
//    res.send("{\"status\":\"rebooting\"}");
//
//    // Allow TCP stack to flush
//    vTaskDelay(pdMS_TO_TICKS(100));
//
//    // Reboot the device
//    esp_restart();
//
//    return common::Result::Ok();
}
} // namespace