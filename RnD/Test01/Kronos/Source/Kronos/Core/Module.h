#pragma once

#include <vector>

class StaticModule
{
public:
    virtual void Initialize() {}
    virtual void Tick() {}
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
