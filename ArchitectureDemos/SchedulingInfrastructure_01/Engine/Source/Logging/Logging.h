#pragma once

#include <string>
#include <iostream>

class Log
{
public:
    inline static void Trace(const std::string& message)
    {
        std::cout << message << "\n";
    }
}

#define KRONOS_CORE_TRACE(x) std::cout << x << "\n"