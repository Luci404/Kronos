/*
Don't mind this attempt, it gets messy with member function pointers all over the place... 
*/

typedef void (*TickFunction)();

class ApplicationScheduler
{
public:
    static void RegisterPlatformInitializeFunction(...);
    static void RegisterPlatformTickFunction(TickFunction* tick);
    static void RegisterPlatformTerminateFunction(...);

    static void RegisterCoreInitializeFunction(...);
    static void RegisterCoreTickFunction(TickFunction* tick);
    static void RegisterCoreTerminateFunction(...);

    static void RegisterSimulationInitializeFunction(...);
    static void RegisterSimulationTickFunction(TickFunction* tick);
    static void RegisterSimulationTerminateFunction(...);
};

class Renderer
{
public:
    Renderer()
    {
        ApplicationScheduler::RegisterSimulationTickFunction(&Tick);
    }

private:
    void Tick()
    {

    }
};