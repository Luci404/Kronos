class ApplicationScheduler
{
    void RegisterPlatformInitializeFunction(...);
    void RegisterCoreInitializeFunction(...);
    void RegisterSimulationInitializeFunction(...);

    void RegisterPlatformTickFunction(...);
    void RegisterCoreTickFunction(...);
    void RegisterSimulationTickFunction(...);

    void RegisterPlatformTerminateFunction(...);
    void RegisterCoreTerminateFunction(...);
    void RegisterSimulationTerminateFunction(...);
};