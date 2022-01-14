#pragma once

#include "KronosEditor/EditorWindow.h"

#include <filesystem>

namespace KronosEditor
{
    class AssetBrowserWindow : public EditorWindow
    {
    public:
        virtual void Render() override;
    private:
        void AssetBrowserWindow::RenderDirectoryRecursive(const std::filesystem::path& directory, bool root = true);
    };
}