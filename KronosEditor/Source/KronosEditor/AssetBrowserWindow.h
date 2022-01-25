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
        void DrawDirectoryRecursive(const std::filesystem::path& directory, bool root = true);
    };
}