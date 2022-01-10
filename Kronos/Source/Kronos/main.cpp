#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <type_traits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args) { return std::make_unique<T>(std::forward<Args>(args)...); }

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

#define KRONOS_CORE_TRACE(x) std::cout << "TRACE: " << x << "\n";
#define KRONOS_CORE_ERROR(x) std::cout << "ERROR: " << x << "\n"

#define KRONOS_CORE_ASSERT(x, ...) { if(!(x)) { KRONOS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

namespace Kronos
{
	class SystemCore
	{
	public:
		virtual void Initialize() = 0;
		virtual void Terminate() = 0;
	};

	class SystemManager : public SystemCore
	{
	public:
		virtual void Initialize() override
		{ 
			KRONOS_CORE_TRACE("Initializing SystemManager...");
		
			for (Scope<SystemCore>& system : m_Systems)
			{
				system->Initialize();
			}
		}

		virtual void Terminate() override
		{
			KRONOS_CORE_TRACE("Terminating SystemManager...");
			
			for (Scope<SystemCore>& system : m_Systems)
			{
				system->Terminate();
			}
		}

		template<typename T>
		void AddSystem()
		{
			KRONOS_CORE_ASSERT((std::is_base_of<SystemCore, T>::value));
			m_Systems.push_back(CreateScope<T>());
		}

	private:
		std::vector<Scope<SystemCore>> m_Systems;
	};

	class EngineSystem : public SystemCore
	{
	};

	class LoggingSystem : public EngineSystem
	{
	public:
		virtual void Initialize() override { KRONOS_CORE_TRACE("Initializing LoggingSystem..."); }
		virtual void Terminate() override { KRONOS_CORE_TRACE("Terminating LoggingSystem..."); }
	};

	class Application
	{
	public:
		Application(const std::string& name = "Kronos Application") 
		{
			m_SystemManager = CreateScope<SystemManager>();
			m_SystemManager->AddSystem<LoggingSystem>();
			m_SystemManager->Initialize();
		}

		virtual ~Application() 
		{
			m_SystemManager->Terminate();
		}

	private:
		Scope<SystemManager> m_SystemManager;
	};
}

namespace KronosEditor
{
	class EditorApplication : public Kronos::Application
	{
	public:
		EditorApplication()
			: Kronos::Application("KronosEditor")
		{
		}
	};

	class EditorWindow
	{
		virtual void Render() = 0;
	};
}

int main(int argc, char* argv[])
{
	KronosEditor::EditorApplication editor;

	GLFWwindow* window;

	// Initialize glfw
	if (!glfwInit()) return -1;

	// Create GLFW window
	window = glfwCreateWindow(1600, 900, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	const char* glslVersion = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Initialize  ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Initialize ImGui platform backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// Initialize glad
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glfwSwapBuffers(window);

		// Call ImGui NewFrame functions
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Render frame
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Submit ImGui commands
		ImGui::ShowDemoWindow();
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

		// Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}

	// Terminate ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}