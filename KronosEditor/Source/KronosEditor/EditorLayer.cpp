#include "KronosEditor/EditorLayer.h"

#include "KronosEditor/AssetBrowserWindow.h"

#include "Kronos/Core/Log.h"

#include <imgui.h>

namespace KronosEditor
{
    void EditorLayer::OnAttach()
    {
        KRONOS_CORE_INFO("EditorLayer::OnAttach")
    
        m_EditorWindows.push_back(new AssetBrowserWindow());
    }

    void EditorLayer::OnDetach()
    {
        KRONOS_CORE_INFO("EditorLayer::OnDetach")
    }

    void EditorLayer::OnImGuiRender()
    {
        // Begin dockspace
        {
            ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace", (bool *)0, window_flags);
            ImGui::PopStyleVar(3);

            ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Render();
        }

        // Window menu bar
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    ImGui::MenuItem("New", "Ctrl+N");
                    ImGui::MenuItem("Open...", "Ctrl+O");
                    ImGui::MenuItem("Save As...", "Ctrl+Shift+S");
                    ImGui::Separator();
                    ImGui::MenuItem("Exit");
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Window"))
                {
                    ImGui::MenuItem("Editor Preferences", NULL, nullptr);
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
        }

        // End dockspace
        {
            ImGui::End(); // Dockspace
        }
    }
}