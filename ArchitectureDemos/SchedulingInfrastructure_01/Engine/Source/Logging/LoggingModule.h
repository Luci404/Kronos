#pragma once

#include "Module.h"
#include "Logging.h"

class LoggingModule : StaticModule
{
public:
    virtual void Initialize() override 
    {
        Log::Trace("Initializing logging module...");
    }

    virtual void Terminate() override
    {
        Log::Trace("Terminateing logging module...");
    }
};