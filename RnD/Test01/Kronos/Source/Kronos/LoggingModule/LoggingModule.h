#pragma once

#include "Kronos/Core/Module.h"

#include <string>
#include <iostream>

namespace Kronos
{
    class LoggingModule : public StaticModule
    {
    public:
        virtual void Initialize() override
        {
            std::cout.flush();
        }

        virtual void Terminate() override {}
    };

    class Log
    {
    public:
        inline static void Trace(const std::string &message)
        {
            std::cout << message << "\n";
        }
    };
}