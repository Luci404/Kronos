#include "KronosEditor/EditorLayer.h"

#include "Kronos/Core/Application.h"

namespace KronosEditor
{
	class EditorApplication : public Kronos::Application
	{
	public:
		virtual void Initialize() override
		{
			Kronos::Application::Initialize();

			PushLayer(new EditorLayer());
		}
	};
}

int main(int argc, char *argv[])
{
	KronosEditor::EditorApplication application;

	application.Initialize();
	while (true)
	{
		application.Update();
	}
	application.Terminate();

	return 0;
}