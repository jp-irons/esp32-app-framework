#pragma once

#include "framework/FrameworkContext.hpp"

class ApplicationContext {
public:
    ApplicationContext();
    ~ApplicationContext() = default;

    void start();
    void loop();   // optional, if your app_main uses it

private:
    framework::FrameworkContext framework;
};