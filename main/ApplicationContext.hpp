#pragma once

#include "framework/FrameworkContext.hpp"

class ApplicationContext {
  public:
    explicit ApplicationContext(framework::FrameworkContext& fw);
    ~ApplicationContext();

    void start();
    void loop(); // optional, if your app_main uses it

  private:
    framework::FrameworkContext& framework;
};