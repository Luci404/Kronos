#include "Kronos/Core/Application.h"

class EditorApplication : Kronos::Application
{
	virtual void Update() override
	{
		Kronos::Application::Update();
	}
};

int main(int argc, char* argv[])
{
	Kronos::Application application;

	application.Initialize();
	while (true) { application.Update(); }
	application.Terminate();

	return 0;
}