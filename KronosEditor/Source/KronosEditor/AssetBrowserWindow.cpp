#include "KronosEditor/AssetBrowserWindow.h"

#include <imgui.h>
#include <set>

namespace KronosEditor
{
	void AssetBrowserWindow::Render()
	{
		if (ImGui::Begin("Asset Browser"))
		{
			//ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(20, 20, 20, 255));
			//ImGui::BeginChild("outer_child", ImVec2(0, ImGui::GetFontSize() * 20.0f), true);
			DrawDirectoryRecursive(std::filesystem::path("C:/Dev/Kronos"), true);
			//ImGui::PopStyleColor(1);
			//ImGui::EndChild();
		}
		ImGui::End();
	}

	// TODO: Shift+Left Click to expand all.

	void AssetBrowserWindow::DrawDirectoryRecursive(const std::filesystem::path& directory, bool root)
	{
		if (root)
		{
			if (!ImGui::TreeNodeEx(directory.filename().string().c_str()))
			{
				return;
			}
		}

		// Directories
		std::set<std::filesystem::path> sortedDirectories;

		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directory))
		{
			if (directoryEntry.is_directory())
			{
				sortedDirectories.insert(directoryEntry.path());
			}
		}

		for (const std::filesystem::path& directoryPath : sortedDirectories)
		{
			if (ImGui::TreeNodeEx(directoryPath.filename().string().c_str()))
			{
				DrawDirectoryRecursive(directoryPath, false);
				ImGui::TreePop();
			}
		}

		// Files
		std::set<std::filesystem::path> sortedFiles;

		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directory))
		{
			if (directoryEntry.is_regular_file())
			{
				sortedFiles.insert(directoryEntry.path());
			}
		}

		for (const std::filesystem::path& filePath : sortedFiles)
		{
			ImGui::Text(filePath.filename().string().c_str());
		}

		if (root)
		{
			ImGui::TreePop();
		}
	}
};