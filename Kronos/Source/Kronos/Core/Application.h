#include "Kronos/Core/Window.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/Core/Log.h"

#include <imgui.h>

namespace Kronos
{
	class Layer
	{
	public:
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
	};

	class EditorLayer : public Layer
	{
		virtual void OnImGuiRender() override
		{
			// Begin dockspace
			{
				ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::Begin("DockSpace", (bool*)0, window_flags);
				ImGui::PopStyleVar(3);

				ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);
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

			// Editor preferences
			{
				if (ImGui::Begin("Editor Preferences"))
				{
					ImGui::TextUnformatted("Test");
				}
				ImGui::End();
			}

			// End dockspace
			{
				ImGui::End(); // Dockspace
			}

		}
	};

	class LayerStack
	{
	public:
		void PushLayer(Layer* layer)
		{
			m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
			m_LayerInsertIndex++;
			layer->OnAttach();
		}

		void PopLayer(Layer* layer)
		{
			auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
			if (it != m_Layers.begin() + m_LayerInsertIndex)
			{
				layer->OnDetach();
				m_Layers.erase(it);
				m_LayerInsertIndex--;
			}
		}

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

		std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end()	const { return m_Layers.end(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

	class Application
	{
	public:
		Application(const std::string& name = "Kronos Application")
		{
		}

		virtual ~Application()
		{
		}

		void Initialize()
		{
			Log::Initialize();

			m_LayerStack.PushLayer(new EditorLayer());

			m_Window = CreateScope<Window>();
		}

		virtual void Update()
		{
			for (Layer* layer : m_LayerStack) { layer->OnUpdate(); }

			m_Window->TMP_ImGuiBegin();
			m_Window->TMP_OpenGlStuff();
			for (Layer* layer : m_LayerStack) { layer->OnImGuiRender(); }
			m_Window->TMP_ImGuiEnd();

			m_Window->Update();
		}

		void Terminate()
		{
		}

	private:
		LayerStack m_LayerStack;
		Scope<Window> m_Window;
	};
}