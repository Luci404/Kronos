#include "Kronos/Core/Window.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/Core/Log.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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

			m_Window = CreateScope<Window>();
		}

		virtual void Update()
		{
			for (Layer* layer : m_LayerStack) { layer->OnUpdate(); }

			m_Window->TMP_ImGuiBegin();
			m_Window->TMP_OpenGlStuff();
			ImGui::ShowDemoWindow();
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