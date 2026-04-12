#include "ApplicationContext.hpp"

#include "logger/Logger.hpp"

static logger::Logger log{"ApplicationContext"};

// TODO is this correct - perhaps needs revisiting
// rootUri must be configured so that Framework and app.js match
// app.js currently expects /framework/api
// ApplicationContext::ApplicationContext()
// apConfig is required
// rootUri is optional. 
// Without rootUri framework will be configured under /framework/api
// 		ApplicationContext::ApplicationContext()
//    		: framework(apConfig) 
// vs
// 		ApplicationContext::ApplicationContext()
//    		: framework(apConfig, '/myrooturi') 
//
ApplicationContext::ApplicationContext()
    : framework(apConfig) {
    log.debug("constructor");
}

void ApplicationContext::start() {
    log.debug("start");
    framework.start();
}

void ApplicationContext::loop() {
    // Optional: forward to WiFiManager or Framework loop if needed
}