#include "KronosEditor/AssetBrowserWindow.h"

#include <imgui.h>

namespace KronosEditor
{
    void AssetBrowserWindow::Render()
    {
        if (ImGui::Begin("Asset Browser"))
        {
            ImGui::TextUnformatted("TODO: Asset Browser");
        }
        ImGui::End();
    }
};