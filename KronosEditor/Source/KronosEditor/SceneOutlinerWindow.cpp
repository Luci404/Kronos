#include "KronosEditor/SceneOutlinerWindow.h"

#include <imgui.h>
#include <memory>

namespace KronosEditor
{
	void SceneOutlinerWindow::Render()
	{
		if (ImGui::Begin("Scene Outliner"))
		{
			if (m_Scene)
			{
				for (const std::unique_ptr<Kronos::Entity>& entity : m_Scene->GetRegistry())
				{
					DrawEntity(*entity.get());
				}
			}
		}
		ImGui::End();
	}

	void SceneOutlinerWindow::SetScene(Kronos::Scene* scene)
	{
		m_Scene = scene;
	}

	void SceneOutlinerWindow::DrawEntity(Kronos::Entity& entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		if (ImGui::TreeNodeEx(entity.GetName().c_str(), flags))
		{
			ImGui::TreePop();
		}

		bool entityDeleted = true;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;

			ImGui::EndPopup();
		}

		if (entityDeleted)
		{
			m_Scene->DestroyEntity(entity);
		}
	}
};