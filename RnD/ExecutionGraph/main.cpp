#include <array>
#include <inttypes.h>
#include <deque>
#include <iostream>
#include <vector>

#define MAXIMUM_TASK_DEPENDENCIES 4

enum class EDependencyType : uint8_t { Precede, Succeeds, Conditional };

class IExecutionNode
{
public:
	virtual void Execute() const = 0;
};
	
struct ExecutionDependency
{
	ExecutionDependency(const EDependencyType dependencyType, const IExecutionNode* dependent, const IExecutionNode* dependency)
		: DependencyType(dependencyType), Dependent(dependent), Dependency(dependency) {}

	const EDependencyType DependencyType;
	const IExecutionNode* Dependent;
    const IExecutionNode* Dependency;
};

class ExecutionTask : public IExecutionNode
{
public: 
    ExecutionTask(void(*function)())
        : m_Function(function) {}

	virtual void Execute() const override { m_Function(); };

private:
	void(*m_Function)();
};


class ExecutionGraph : public IExecutionNode
{
public:
    ExecutionGraph() : m_DebugName("UNNAMED") {}
    ExecutionGraph(const std::string& debugName) : m_DebugName(debugName) {}

	template<EDependencyType DependencyType>
	void AddDependency(const IExecutionNode* dependent, const IExecutionNode* dependency)
	{
		m_ExecutionDependencies.push_back(ExecutionDependency(DependencyType, dependent, dependency));
	}

	virtual void Execute() const override
    {
        std::cout << m_DebugName << std::endl;

        for (const IExecutionNode* executionNode : m_ExecutionNodes)
        {
            executionNode->Execute();
        }
    }
	
	IExecutionNode* AppendTask(void(*function)())
    {
        ExecutionTask* executionNode = new ExecutionTask(function);
        m_ExecutionNodes.push_back(static_cast<IExecutionNode*>(executionNode));
        return executionNode;    
    }

	IExecutionNode* AppendGraph(ExecutionGraph* executionGraph)
    {
        m_ExecutionNodes.push_back(static_cast<IExecutionNode*>(executionGraph));
        return executionGraph;
    }

private:
	std::deque<IExecutionNode*> m_ExecutionNodes;
	std::deque<ExecutionDependency> m_ExecutionDependencies;

    const std::string m_DebugName;
};

class ExecutionScheduler
{
public:
    static void AppendGraph(ExecutionGraph* executionGraph)
    {
        m_ExecutionNodes.push_back(static_cast<IExecutionNode*>(executionGraph));
    }

private:
    static std::vector<IExecutionNode*> m_ExecutionNodes;
};

std::vector<IExecutionNode*> ExecutionScheduler::m_ExecutionNodes = std::vector<IExecutionNode*>();


/// EXAMPLE

class RendererModule
{
public:
    static void BuildSceneData() { std::cout << "RendererModule::BuildSceneData()" << std::endl; }
    static void BuildCommandBuffers() { std::cout << "RendererModule::BuildCommandBuffers()" << std::endl; }
    static void RenderScene() { std::cout << "RendererModule::RenderScene()" << std::endl; }
    static void QueueNextFrame() {std::cout << "RendererModule::QueueNextFrame()" << std::endl; }

	static void Initialize()
	{
		ExecutionGraph rendererLoopExecutionGraph = ExecutionGraph("Renderer Loop");
		IExecutionNode* buildSceneDataNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::BuildSceneData);
		IExecutionNode* buildCommandBuffersNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::BuildCommandBuffers);
		IExecutionNode* renderSceneNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::RenderScene);
		IExecutionNode* queueNextFrameNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::QueueNextFrame);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Precede>(buildCommandBuffersNode, buildSceneDataNode);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Precede>(renderSceneNode, buildCommandBuffersNode);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Precede>(queueNextFrameNode, renderSceneNode);
		//rendererLoopExecutionGraph.AddDependency<EDependencyType::Conditional>(queueNextFrameNode, RendererModule::ShouldQueueNextFrame);
		
		ExecutionScheduler::AppendGraph(&rendererLoopExecutionGraph);
	}
	
	static void Terminate() { /* ... */ }
};

class SceneModule
{
public:
	static void Initialize() { /* ... */ }
	
	static void Terminate() { /* ... */ }
};

int main(int argc, char** argv)
{
	ExecutionGraph initializationExecutionGraph = ExecutionGraph("Application Initialization");
	IExecutionNode* rendererModuleInitializationNode = initializationExecutionGraph.AppendTask(&RendererModule::Initialize);
	IExecutionNode* sceneModuleInitializationNode = initializationExecutionGraph.AppendTask(&SceneModule::Initialize);
	initializationExecutionGraph.AddDependency<EDependencyType::Precede>(rendererModuleInitializationNode, sceneModuleInitializationNode);

	ExecutionGraph terminationExecutionGraph = ExecutionGraph("Application Termination");
	IExecutionNode* rendererModuleTerminationNode = terminationExecutionGraph.AppendTask(&RendererModule::Terminate);
	IExecutionNode* sceneModuleTerminationNode = terminationExecutionGraph.AppendTask(&SceneModule::Terminate);
	terminationExecutionGraph.AddDependency<EDependencyType::Succeeds>(rendererModuleTerminationNode, sceneModuleTerminationNode);
	
	ExecutionGraph applicationExecutionGraph = ExecutionGraph("Application Master");
	IExecutionNode* initializationNode = applicationExecutionGraph.AppendGraph(&initializationExecutionGraph);
	IExecutionNode* terminationNode = applicationExecutionGraph.AppendGraph(&terminationExecutionGraph);
	//applicationExecutionGraph.AddDependency<EDependencyType::Conditional>(terminationNode, &Core::ShouldClose);
	applicationExecutionGraph.AddDependency<EDependencyType::Precede>(terminationNode, initializationNode);
	
	ExecutionScheduler::AppendGraph(&applicationExecutionGraph);
}