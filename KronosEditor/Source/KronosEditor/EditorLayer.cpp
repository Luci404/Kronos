#include "KronosEditor/EditorLayer.h"

#include "KronosEditor/AssetBrowserWindow.h"
#include "KronosEditor/SceneOutlinerWindow.h"

#include "Kronos/Core/Log.h"
#include "Kronos/Scene/Entity.h" // TMP

#include <imgui.h>

namespace KronosEditor
{
    void EditorLayer::OnAttach()
    {
        KRONOS_CORE_INFO("EditorLayer::OnAttach")
    
        m_EditorWindows.push_back(new AssetBrowserWindow());
        SceneOutlinerWindow* sceneOutlinerWindow = static_cast<SceneOutlinerWindow*>(m_EditorWindows.emplace_back(new SceneOutlinerWindow()));
        sceneOutlinerWindow->SetScene(&ActiveScene);

        ActiveScene.CreateEntity<Kronos::Entity>("Entity01"); // TMP
        ActiveScene.CreateEntity<Kronos::Entity>("Entity02"); // TMP
        ActiveScene.CreateEntity<Kronos::Entity>("Entity03"); // TMP
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

        ImGui::ShowDemoWindow();

        // Todo list
        {
            ImGui::Begin("TODO");
            ImGui::BulletText("Hot-reloading");
            ImGui::BulletText("Unified architecture");
            ImGui::End();
        }

        // End dockspace
        {
            ImGui::End(); // Dockspace
        }
    }
}