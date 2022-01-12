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

			// Load fonts
			// TODO: Don't use absolute paths...
			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Dev/Kronos/KronosEditor/Assets/Fonts/Consolas/CONSOLA.TTF", 14.0f);
			
			// Configure style variables
			ImGuiStyle& style = ImGui::GetStyle();
			// Main
			style.IndentSpacing = 16;
			style.ScrollbarSize = 12;
			// Borders
			style.WindowBorderSize = 0;
			style.PopupBorderSize = 0;
			style.FrameBorderSize= 0;
			style.TabBorderSize= 0;
			// Rounding
			style.FrameRounding = 2;
			style.GrabRounding = 2;
			// Alignment
			style.WindowMenuButtonPosition = ImGuiDir_Right;
			// Colors
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_WindowBg] = ImColor(20, 20, 20, 255);
			colors[ImGuiCol_Button] = ImColor(30, 30, 30, 255);
			colors[ImGuiCol_FrameBg] = ImColor(35, 35, 35, 255);
			colors[ImGuiCol_FrameBgHovered] = ImColor(65, 65, 65, 255);
			colors[ImGuiCol_FrameBgActive] = ImColor(45, 45, 45, 255);
			colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 255);
			colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 255);
			colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 255);
			colors[ImGuiCol_ButtonHovered] = ImColor(40, 40, 40, 255);
			colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60, 255);
			colors[ImGuiCol_Header] = ImColor(35, 35, 35, 255);
			colors[ImGuiCol_HeaderHovered] = ImColor(45, 45, 45, 255);
			colors[ImGuiCol_HeaderActive] = ImColor(65, 65, 65, 255);
			colors[ImGuiCol_Separator] = ImColor(35, 35, 35, 255);
			colors[ImGuiCol_SeparatorHovered] = ImColor(45, 45, 45, 255);
			colors[ImGuiCol_SeparatorActive] = ImColor(55, 55, 55, 255);
			colors[ImGuiCol_Tab] = ImColor(35, 35, 35, 255);
			colors[ImGuiCol_TabHovered] = ImColor(65, 65, 65, 255);
			colors[ImGuiCol_TabActive] = ImColor(45, 45, 45, 255);
			colors[ImGuiCol_TabUnfocused] = ImColor(35, 35, 35, 255);
			colors[ImGuiCol_TabUnfocusedActive] = ImColor(35, 35, 35, 255);
			colors[ImGuiCol_CheckMark] = ImColor(160, 160, 160, 255);





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