#pragma once

#include "KronosEditor/EditorWindow.h"

#include "Kronos/Core/Layer.h"

#include <vector>

namespace KronosEditor
{
    class EditorLayer : public Kronos::Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
    private:
        std::vector<EditorWindow*> m_EditorWindows;
    };
}