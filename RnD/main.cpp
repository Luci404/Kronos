#include <string>

class StaticModule 
{
public:
    virtual void Initialize() {}
    virtual void Terminate() {}
};

class ModuleStack
{
public:
    StaticModule* PushModule(StaticModule* module)
    {
        m_Modules.push_back(module);
        return module;
    }

    std::vector<StaticModule*>::iterator begin() { return m_Modules.begin(); }
    std::vector<StaticModule*>::iterator end() { return m_Modules.end(); }
    std::vector<StaticModule*>::reverse_iterator rbegin() { return m_Modules.rbegin(); }
    std::vector<StaticModule*>::reverse_iterator rend() { return m_Modules.rend(); }

    std::vector<StaticModule*>::const_iterator begin() const { return m_Modules.begin(); }
    std::vector<StaticModule*>::const_iterator end() const { return m_Modules.end(); }
    std::vector<StaticModule*>::const_reverse_iterator rbegin() const { return m_Modules.rbegin(); }
    std::vector<StaticModule*>::const_reverse_iterator rend() const { return m_Modules.rend(); }

private:
    std::vector<StaticModule *> m_Modules;
};


class Layer
{
public:
    virtual void Initialize() {}
    virtual void Tick() {}
    virtual void Terminate() {}

protected:
    ModuleStack m_ModuleStack;
};

#pragma region LevelStreamingModule 
class LevelStreamingModule final : public StaticModule
{
public:
    virtual void Initialize() {}
    virtual void Terminate() {}
};

class Level {};

class LevelStreamingManager
{
public:
    static void LoadLevel(const Level& level) {}
    static void UnloadLevel(const Level& level) {}
};

#pragma endregion

#pragma region RendererModule
class RendererModule final : public StaticModule
{
public:
    virtual void Initialize() {}
    virtual void Terminate() {}
};
#pragma endregion

#pragma region GameplayFrameworkModule
class GameplayFrameworkModule final : public StaticModule
{
public:
    virtual void Initialize() {}
    virtual void Terminate() {}
};

class PlayerPawn {};
class PlayerController {};

class GameMode
{
public:
    virtual void StartGame() 
    {

    }
};

class GameState {};
class PlayerState {};
class HUD {};
class GameInstance
{
public:
    virtual void StartGame()
    {
        m_EntryLevel = Level::CreateLevelFromFile("level.lvl"); 
        LevelStreamingManager::LoadLevel(m_EntryLevel);
    }

private:
    Level m_EntryLevel;
};

#pragma endregion

class SimulationLayer final : public Layer
{
public:
    virtual void Initialize()
    {
        m_ModuleStack.PushModule(new LevelStreamingModule());
        m_ModuleStack.PushModule(new GameplayFrameworkModule());
    }
};

class ApplicationLayer final : public Layer
{
public:
    virtual void Initialize()
    {
    }
};