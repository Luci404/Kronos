#include "KronosEditor/AssetBrowserWindow.h"

#include <imgui.h>
#include <set>

namespace KronosEditor
{
	void AssetBrowserWindow::Render()
	{
		ImGui::ShowDemoWindow();

		if (ImGui::Begin("Asset Browser"))
		{
			RenderDirectoryRecursive(std::filesystem::path("C:/Dev/Kronos/"), true);
		}
		ImGui::End();
	}

	// TODO: Shift+Left Click to expand all.

	void AssetBrowserWindow::RenderDirectoryRecursive(const std::filesystem::path& directory, bool expanded)
	{
		if (ImGui::TreeNodeEx(directory.parent_path().filename().string().c_str()))
		{
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
					RenderDirectoryRecursive(directoryPath, expanded);
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

			ImGui::TreePop();
		}
	}
};