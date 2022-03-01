#pragma once

#include "Kronos/RendererModule/RendererCore.h"

namespace Kronos
{
    class ImGuiRenderer : public RendererCore
    {
    public:
        ImGuiRenderer();
        ~ImGuiRenderer();
    };
}