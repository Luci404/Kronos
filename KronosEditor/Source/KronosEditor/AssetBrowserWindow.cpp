#include "KronosEditor/AssetBrowserWindow.h"

#include <imgui.h>

namespace KronosEditor
{
    void AssetBrowserWindow::Render()
    {
        if (ImGui::Begin("Editor Preferences"))
        {
            ImGui::TextUnformatted("Test");
        }
        ImGui::End();
    }
};