#include "ApplicationContext.hpp"

#include "logger/Logger.hpp"

static logger::Logger log{"ApplicationContext"};

ApplicationContext::ApplicationContext(framework::FrameworkContext& fw)
    : framework(fw) {
    log.debug("constructor");
}

ApplicationContext::~ApplicationContext() {
	log.info("destructor");
}


void ApplicationContext::start() {
    log.debug("start");
    framework.start();
}

void ApplicationContext::loop() {
    // Optional: forward to WiFiManager or Framework loop if needed
}