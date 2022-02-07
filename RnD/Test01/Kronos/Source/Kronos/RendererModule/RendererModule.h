#pragma once

#include "Kronos/WindowModule/WindowModule.h"
#include "Kronos/IntegrantModule/IntegrantModule.h"
#include "Kronos/Core/Module.h"
#include "Kronos/Core/Memory.h"
#include "Kronos/Core/Assert.h"

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// https://www.cg.tuwien.ac.at/research/publications/2007/bauchinger-2007-mre/bauchinger-2007-mre-Thesis.pdf

namespace Lada
{
    namespace RHI
    {
        enum class ERendererPlatform
        {
            Vulkan,
            OpenGL
        };

        class ISurface {};
        class IDevice {};
        class ICommandList {};
        class IGraphicsPipeline {};
        class IComputePipeline {};
    
        namespace VulkanRHI
        {
            class VulkanSurface : public ISurface {};
            class VulkanDevice : public IDevice {};
            class VulkanCommandList : public ICommandList {};
            class VulkanGraphicsPipeline : public IGraphicsPipeline {};
            class VulkanComputePipeline : public IComputePipeline {};
        }

        namespace OpenGLRHI
        {
            class OpenGLSurface : public ISurface {};
            class OpenGLDevice : public IDevice {};
            class OpenGLCommandList : public ICommandList {};
            class OpenGLGraphicsPipeline : public IGraphicsPipeline {};
            class OpenGLComputePipeline : public IComputePipeline {};
        }

        class GraphicsContext
        {
        public:
            GraphicsContext() {}
            void Submit() {}
        };
    }

    class Mesh {};
    class Texture {};
    class Material {};
    class RendererCore {};
    class ForwardRenderer : public RendererCore {};
    class DeferredRenderer : public RendererCore {};
    
    class Camera {};

    class RenderGraph {};
    class RenderGraphNode {};
}

namespace Kronos
{
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    const char* vertexShaderSource = "#version 450 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout(std140, binding = 0) uniform Matrices\n"
        "{\n"
        "   mat4 projection; \n"
        "   mat4 view; \n"
        "}; \n"
        "layout(std140, binding = 1) uniform Object\n"
        "{\n"
        "   mat4 model; \n"
        "}; \n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 450 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.32f, 0.32f, 1.0f);\n"
        "}\n\0";

    class StaticMeshIntegrant : public IIntegrant
    {
    public:
        StaticMeshIntegrant()
            : TransformMatrix(glm::mat4(1.0f))
        {
        }
    public:
        glm::mat4 TransformMatrix;
    };

    class SceneRenderer /* : public Lada::RenderGraph */
    {
    public:
        SceneRenderer(Ref<Window> window)
            : m_Window(window)
        {
            PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
                PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                32,                   // Colordepth of the framebuffer.
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,                   // Number of bits for the depthbuffer
                8,                    // Number of bits for the stencilbuffer
                0,                    // Number of Aux buffers in the framebuffer.
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            // Create, and make context current.
            ourWindowHandleToDeviceContext = m_Window->GetDeviceContext_TMP();

            int letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pixelFormatDescriptor);
            SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pixelFormatDescriptor);

            ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
            wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

            // Initialize glad
            KRONOS_CORE_ASSERT(gladLoadGL(), "Failed to initialize OpenGL context.");

            glEnable(GL_DEPTH_TEST);

            // build and compile our shader program
            // ------------------------------------
            // vertex shader
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);
            // check for shader compile errors
            int success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            // fragment shader
            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);
            // check for shader compile errors
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            // link shaders
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            // check for linking errors
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            float cubeVertices[] = {
                // positions         
                -0.5f, -0.5f, -0.5f,
                 0.5f, -0.5f, -0.5f,
                 0.5f,  0.5f, -0.5f,
                 0.5f,  0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,

                -0.5f, -0.5f,  0.5f,
                 0.5f, -0.5f,  0.5f,
                 0.5f,  0.5f,  0.5f,
                 0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                -0.5f, -0.5f,  0.5f,

                -0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,

                 0.5f,  0.5f,  0.5f,
                 0.5f,  0.5f, -0.5f,
                 0.5f, -0.5f, -0.5f,
                 0.5f, -0.5f, -0.5f,
                 0.5f, -0.5f,  0.5f,
                 0.5f,  0.5f,  0.5f,

                -0.5f, -0.5f, -0.5f,
                 0.5f, -0.5f, -0.5f,
                 0.5f, -0.5f,  0.5f,
                 0.5f, -0.5f,  0.5f,
                -0.5f, -0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,

                -0.5f,  0.5f, -0.5f,
                 0.5f,  0.5f, -0.5f,
                 0.5f,  0.5f,  0.5f,
                 0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,
            };
            // cube VAO
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            glBindVertexArray(cubeVAO);
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
            glBindVertexArray(0);

            // configure a uniform buffer object
            // ---------------------------------
            // first. We get the relevant block indices
            /*glUniformBlockBinding(shaderProgram, glGetUniformBlockIndex(shaderProgram, "Matrices"), 0);
            glGenBuffers(1, &uboMatrices);
            glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
            glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));*/
            glCreateBuffers(1, &uboMatrices);
            glNamedBufferData(uboMatrices, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);

            // Object uniform buffer
            /*glUniformBlockBinding(shaderProgram, glGetUniformBlockIndex(shaderProgram, "Object"), 1);
            glGenBuffers(1, &uboObject);
            glBindBuffer(GL_UNIFORM_BUFFER, uboObject);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboObject, 0, sizeof(glm::mat4));*/
            glCreateBuffers(1, &uboObject);
            glNamedBufferData(uboObject, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboObject);

            m_StaticMeshes[0] = StaticMeshIntegrant();
            m_StaticMeshes[1] = StaticMeshIntegrant();
            m_StaticMeshes[2] = StaticMeshIntegrant();
            m_StaticMeshes[3] = StaticMeshIntegrant();

            m_StaticMeshes[0].TransformMatrix = glm::translate(m_StaticMeshes[0].TransformMatrix, glm::vec3(-0.75f, 0.75f, 0.0f));
            m_StaticMeshes[1].TransformMatrix = glm::translate(m_StaticMeshes[1].TransformMatrix, glm::vec3(0.75f, 0.75f, 0.0f));
            m_StaticMeshes[2].TransformMatrix = glm::translate(m_StaticMeshes[2].TransformMatrix, glm::vec3(-0.75f, -0.75f, 0.0f));
            m_StaticMeshes[3].TransformMatrix = glm::translate(m_StaticMeshes[3].TransformMatrix, glm::vec3(0.75f, -0.75f, 0.0f));

            //
            InitVulkan();
        }

        void InitVulkan()
        {
            // Create instance.
            VkApplicationInfo applicationInfo{};
            applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.pNext = nullptr;
            applicationInfo.pApplicationName = "Kronos Application";
            applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            applicationInfo.pEngineName = "Kronos Engine";
            applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            applicationInfo.apiVersion = VK_API_VERSION_1_0;
        
            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pNext = nullptr;
            instanceCreateInfo.flags = 0;
            instanceCreateInfo.pApplicationInfo = &applicationInfo;
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.ppEnabledLayerNames = nullptr;
            instanceCreateInfo.enabledExtensionCount = 0;
            instanceCreateInfo.ppEnabledExtensionNames = nullptr;

            KRONOS_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create instance!");

            // Select physical device.
            uint32_t physicalDeviceCount = 0;
            vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
            KRONOS_CORE_ASSERT(physicalDeviceCount > 0, "Failed to find GPU with Vulkan support!");

            std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
            vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());
            m_PhysicalDevice = physicalDevices[0]; // TODO: Selection function.
            KRONOS_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
        
            // Find queue families.
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

            for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
            {
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) m_GraphicsQueueFamilyIndex = i;
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) m_GraphicsQueueFamilyIndex = i;
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) m_GraphicsQueueFamilyIndex = i;
            }


            // Create logical device.
            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.pNext = nullptr;
            deviceCreateInfo.flags = 0;
            deviceCreateInfo.queueCreateInfoCount = 0;
            deviceCreateInfo.pQueueCreateInfos = nullptr;
            deviceCreateInfo.enabledLayerCount = 0;
            deviceCreateInfo.ppEnabledLayerNames = nullptr;
            deviceCreateInfo.enabledExtensionCount = 0;
            deviceCreateInfo.ppEnabledExtensionNames = nullptr;

            KRONOS_CORE_ASSERT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");
        }
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_LogicalDevice;
        uint32_t m_GraphicsQueueFamilyIndex;
        uint32_t m_ComputeQueueFamilyIndex;
        uint32_t m_TransferQueueFamilyIndex;


        ~SceneRenderer()
        {
            wglMakeCurrent(ourWindowHandleToDeviceContext, NULL);
            wglDeleteContext(ourOpenGLRenderingContext);
        }

        void Render()
        {
            glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Camera */
            { 
                glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
                glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }

            for (const StaticMeshIntegrant staticMesh : m_StaticMeshes)
            {
                glBindVertexArray(cubeVAO);
                glUseProgram(shaderProgram);

                glBindBuffer(GL_UNIFORM_BUFFER, uboObject);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(staticMesh.TransformMatrix));
                glBindBuffer(GL_UNIFORM_BUFFER, 0);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            SwapBuffers(m_Window->GetDeviceContext_TMP());
        }
    
    private:
        void OnWindowResize(uint32_t newWidth, uint32_t newHeight)
        {
            glViewport(0, 0, newWidth, newHeight);
        }

    private:
        Ref<Window> m_Window;

        StaticMeshIntegrant m_StaticMeshes[4];

        unsigned int uboMatrices, uboObject;
        unsigned int cubeVBO, cubeVAO;
        unsigned int shaderProgram;
        HDC ourWindowHandleToDeviceContext;
        HGLRC ourOpenGLRenderingContext;
};

    class RendererModule : public StaticModule {};
}