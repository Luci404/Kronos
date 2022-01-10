#include <Kronos/Core/Window.h>

#include <Kronos/Core/Log.h>
#include <Kronos/Core/Assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace Kronos
{

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		KRONOS_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const std::string& title, uint32_t width, uint32_t height, uint32_t xPosition, uint32_t yPosition, bool maximized, bool vSync)
	{
		// Initialize GLFW
		if (s_GLFWWindowCount == 0)
		{
			if (!glfwInit())
			{
				glfwTerminate();
				KRONOS_CORE_ASSERT(false);
				return;
			}
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		// Create windows
		m_GLFWWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		s_GLFWWindowCount++;
		glfwMakeContextCurrent(m_GLFWWindow);
		glfwSwapInterval(vSync ? 1 : 0); // Enable vsync

		const char* glslVersion = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		// Initialize  ImGui
		if (s_GLFWWindowCount == 1)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			// Initialize ImGui platform backends
			ImGui_ImplGlfw_InitForOpenGL(m_GLFWWindow, true);
			ImGui_ImplOpenGL3_Init(glslVersion);

			// Initialize glad
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		}
	}

	void Window::Update()
	{
		glfwMakeContextCurrent(m_GLFWWindow);
		glfwPollEvents();
		glfwSwapBuffers(m_GLFWWindow);
	}

	void Window::TMP_ImGuiBegin()
	{
		// Call ImGui NewFrame functions
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Window::TMP_ImGuiEnd()
	{
		// Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Window::TMP_OpenGlStuff()
	{
		int display_w, display_h;
		glfwGetFramebufferSize(m_GLFWWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_GLFWWindow);
		s_GLFWWindowCount--;

		if (s_GLFWWindowCount == 0)
		{
			// Terminate ImGui
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			// Terminate GLFW
			glfwDestroyWindow(m_GLFWWindow);
			glfwTerminate();
		}
	}
}