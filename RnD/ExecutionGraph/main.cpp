#include <array>
#include <inttypes.h>
#include <deque>

#define MAXIMUM_TASK_DEPENDENCIES 4

enum class EDependencyType : uint8_t { Precede, Succeeds, Conditional };

class IExecutionNode
{
public:
	virtual void Execute() const = 0;
};
	
struct ExecutionDependency
{
	ExecutionDependency(EDependencyType dependencyType, IExecutionNode* dependent, IExecutionNode* dependency)
		: DependencyType(dependencyType), Dependent(dependent), Dependency(dependency) {}

	EDependencyType DependencyType;
	IExecutionNode* Dependent;
    IExecutionNode* Dependency;
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
	template<EDependencyType DependencyType>
	void AddDependency(const IExecutionNode* dependent, const IExecutionNode* dependency)
	{
		m_ExecutionDependencies.push_back(ExecutionDependency(DependencyType, dependent, dependency));
	}

	virtual void Execute() const override { };
	
	ExecutionTask* AppendTask(void(*function)())
    {
        ExecutionTask* executionNode = new ExecutionTask(function);
        m_ExecutionNodes.push_back(static_cast<IExecutionNode*>(executionNode));
        return executionNode;    
    }

	void AppendGraph(ExecutionGraph* executionGraph)
    {
        m_ExecutionNodes.push_back(static_cast<IExecutionNode*>(executionGraph));
    }
	
	void Execute() { /* ... */ }

private:
	std::deque<IExecutionNode*> m_ExecutionNodes;
	std::deque<ExecutionDependency> m_ExecutionDependencies;
};

class RendererModule
{
public:
    static void BuildSceneData() {}
    static void BuildCommandBuffers() {}
    static void RenderScene() {}
    static void QueueNextFrame() {}

	static void Initialize()
	{
		ExecutionGraph rendererLoopExecutionGraph;
		ExecutionTask* buildSceneDataNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::BuildSceneData);
		ExecutionTask* buildCommandBuffersNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::BuildCommandBuffers);
		ExecutionTask* renderSceneNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::RenderScene);
		ExecutionTask* queueNextFrameNode = rendererLoopExecutionGraph.AppendTask(&RendererModule::QueueNextFrame);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Precede>(buildCommandBuffersNode, buildSceneDataNode);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Precede>(renderSceneNode, buildCommandBuffersNode);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Precede>(queueNextFrameNode, renderSceneNode);
		rendererLoopExecutionGraph.AddDependency<EDependencyType::Conditional>(queueNextFrameNode, &RendererModule::ShouldQueueNextFrame);
		
		ExecutionScheduler::AppendGraph(rendererLoopExecutionGraph);
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
	ExecutionGraph initializationExecutionGraph;
	ExecutionNode rendererModuleInitializationNode = initializationExecutionGraph.AppendTask(&RendererModule::Initialize);
	ExecutionNode sceneModuleInitializationNode = initializationExecutionGraph.AppendTask(&SceneModule::Initialize);
	initializationExecutionGraph.AddDependency<EDependencyType::Precede>(rendererModuleInitializationNode, sceneModuleInitializationNode);

	ExecutionGraph terminationExecutionGraph;
	ExecutionNode rendererModuleTerminationNode = terminationExecutionGraph.AppendTask(&RendererModule::Terminate);
	ExecutionNode sceneModuleTerminationNode = terminationExecutionGraph.AppendTask(&SceneModule::Terminate);
	terminationExecutionGraph.AddDependency<EDependencyType::Succeeds>(rendererModuleTerminationNode, sceneModuleTerminationNode);
	
	ExecutionGraph applicationExecutionGraph;
	ExecutionNode initializationNode = applicationExecutionGraph.AppendGraph(initializationExecutionGraph);
	ExecutionNode terminationNode = applicationExecutionGraph.AppendGraph(terminationExecutionGraph);
	applicationExecutionGraph.AddDependency<EDependencyType::Conditional>(terminationNode, &Core::ShouldClose);
	applicationExecutionGraph.AddDependency<EDependencyType::Precede>(terminationNode, initializationNode);
	
	ExecutionScheduler::AppendGraph(applicationExecutionGraph);
}