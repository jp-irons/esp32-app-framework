#include "device/DeviceApiHandler.hpp"
#include "http/HttpMethod.hpp"
#include "logger/Logger.hpp"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace device {
	
static logger::Logger log{"DeviceApiHandler"};

DeviceApiHandler::DeviceApiHandler() {
	log.debug("constructor");
}

using namespace common;
using namespace http;

Result DeviceApiHandler::handle(HttpRequest& req, HttpResponse& res) {
	log.debug("handle");
	
	HttpMethod method = req.method();
	switch (method) {
		case HttpMethod::Get:
			return handleGet(req, res);
		case HttpMethod::Post:
			return handlePost(req, res);
		default:
			res.sendJsonError(405, std::string("Method") + toString(method) + "not found");
			return common::Result::Ok;
	}
}

Result DeviceApiHandler::handleGet(http::HttpRequest& req, http::HttpResponse& res) {
	res.sendJsonError(501, "Not implemented");
	return Result::Ok;
}
	
Result DeviceApiHandler::handlePost(http::HttpRequest& req, http::HttpResponse& res) {
    const std::string action = http::HttpHandler::extractAction(req.path());
	
    if (action == "reboot") {
        return handleReboot(req, res);
    }
	if (action == "clearNvs") {
	    return handleClearNvs(req, res);
	}
	res.sendNotFound404("target '" + action + "' not found");
    return Result::Ok;
}

Result DeviceApiHandler::handleClearNvs(http::HttpRequest& req, http::HttpResponse& res) {
    log.info("handleClearNvs not implemented");
	Result r = deviceService.clearNvs();
    if (r != common::Result::Ok) {
        res.sendJsonError(500, std::string("Error ") + toString(r) + " clearing NVS");
		return Result::Ok;
	}
    res.sendJsonOk("NVS cleared");
	return Result::Ok;
}

Result DeviceApiHandler::handleReboot(http::HttpRequest& req, http::HttpResponse& res)
{
	log.debug("handleReboot");
	if (req.method()!= http::HttpMethod::Post) {
		res.sendJsonError(405, "{\"error\":\"method not allowed\"}");
		return Result::Ok;
	}
    // Respond BEFORE rebooting
	res.sendJson("{\"status\":\"rebooting\"}");
    // Allow TCP stack to flush
	log.debug("waiting 500ms for TCP stack to flush");

    vTaskDelay(pdMS_TO_TICKS(500));
	log.info("rebooting device");
    // Reboot the device
    esp_restart();

    return common::Result::Ok;
}

} // namespace