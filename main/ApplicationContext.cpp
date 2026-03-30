#include "ApplicationContext.hpp"

ApplicationContext::ApplicationContext()
    : framework()
{
}

void ApplicationContext::start()
{
    framework.start();
}

void ApplicationContext::loop()
{
    // Optional: forward to WiFiManager loop if needed
    // framework.loop();  // if you add one later
}