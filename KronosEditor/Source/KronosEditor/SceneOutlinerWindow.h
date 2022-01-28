#pragma once

#include "KronosEditor/EditorWindow.h"

// #include "Kronos/Scene/Scene.h"

namespace KronosEditor
{
	class SceneOutlinerWindow : public EditorWindow
	{
	public:
		virtual void Render() override;
		// void SetScene(Kronos::Scene* scene);

	private:
		// void DrawEntity(Kronos::Entity& entity);

	private:
		// Kronos::Scene* m_Scene;
	};
}