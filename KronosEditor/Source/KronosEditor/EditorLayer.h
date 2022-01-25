#pragma once

#include "KronosEditor/EditorWindow.h"

#include "Kronos/Core/Layer.h"
#include "Kronos/Scene/Scene.h" // TMP

#include <vector>

namespace KronosEditor
{
    class EditorLayer : public Kronos::Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;

    public:
        //TMP
        Kronos::Scene ActiveScene;

    private:
        std::vector<EditorWindow*> m_EditorWindows;
    };
}