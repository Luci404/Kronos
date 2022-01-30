#pragma once

#include "../Core/Module.h"

#include <string>
#include <iostream>

class LoggingModule : public StaticModule
{
public:
    virtual void Initialize() override {}

    virtual void Terminate() override {}
};

class Log
{
public:
    inline static void Trace(const std::string& message)
    {
        std::cout << message << "\n";
    }
};

#define KRONOS_CORE_TRACE(x) std::cout << x << "\n"