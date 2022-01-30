#pragma once

#include "Kronos/Core/Window.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/Core/Log.h"
#include "Kronos/Core/Layer.h"

namespace Kronos
{
	class Application
	{
	public:
		Application(const std::string& name = "Kronos Application") {}

		virtual ~Application() {}

		virtual void Initialize()
		{
			Log::Initialize();

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

		virtual void Terminate()
		{
		}

		void PushLayer(Layer* layer)
		{
            m_LayerStack.PushLayer(layer);
		}

	private:
		LayerStack m_LayerStack;
		Scope<Window> m_Window;
	};
}