#include "Kronos/RendererModule/Vulkan/VulkanBuffer.h"

#include "Kronos/Core/Memory.h"

namespace Kronos
{
    struct Vertex
    {

    };

    class StaticMesh
    {
    public:
        StaticMesh(const VulkanDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    private:
		Scope<VulkanBuffer> m_VertexBuffer;
		Scope<VulkanBuffer> m_IndexBuffer;
    };
}