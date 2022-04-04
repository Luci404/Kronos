#include <array>
#include <inttypes.h>
#include <deque>
#include <iostream>
#include <vector>

#include <thread>
#include <algorithm>
#include <sstream>
#include <mutex>
#include <functional>
#include <memory>
#include <atomic>

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

struct SpinLock
{
	/*
	TODO: Consider using the PAUSE instruction to avoid blocking other CPU cores sharing the same load-store uint.
	Refer to the "Reducing load-store unit utilization" section of "Correctly implementing a spinlock in C++" by Erik Rigtorp (https://rigtorp.se/spinlock/).
	*/
	void Lock() noexcept
	{
		for (;;) {
			// Optimistically assume the lock is free on the first try
			if (!m_Lock.exchange(true, std::memory_order_acquire)) {
				return;
			}
			// Wait for lock to be released without generating cache misses
			while (m_Lock.load(std::memory_order_relaxed));
		}
	}

	bool TryLock() noexcept {
		// First do a relaxed load to check if lock is free in order to prevent
		// unnecessary cache misses if someone does while(!try_lock())
		return !m_Lock.load(std::memory_order_relaxed) && !m_Lock.exchange(true, std::memory_order_acquire);
	}

	void Unlock() noexcept
	{
		m_Lock.store(false, std::memory_order_release);
	}

private:
	std::atomic<bool> m_Lock = { false };
};

class ExecutionScheduler
{
public:
    static void Run()
    {
        uint32_t threadCount = std::max(1u, std::thread::hardware_concurrency());

        std::cout << (std::ostringstream{} << "Thread count: " << threadCount << '\n').str();

        for (uint32_t i = 0; i < threadCount; ++i)
        {
            std::thread worker([](uint32_t threadIndex) {
                while(true)
                {
                    // std::cout << (std::ostringstream{} << "Thread " << threadIndex << '\n').str();
                    while(m_ExecutionNodes.size() > 0)
                    {
                        IExecutionNode* executionNode;

                        m_ExecutionNodesLock.Lock();
                        if (m_ExecutionNodes.size() <= 0) continue;
                        executionNode = std::move(m_ExecutionNodes.front());
                        m_ExecutionNodes.pop_front();
                        m_ExecutionNodesLock.Unlock();

                        std::cout << (std::ostringstream{} << "[" << threadIndex << "] Executing Node | Node: " << reinterpret_cast<void*>(executionNode) << '\n').str();
                        executionNode->Execute();
                    }
                }
            }, i);
        
            worker.detach();
        }

        while(true) {}
    }

    static void AppendGraph(ExecutionGraph* executionGraph)
    {
        m_ExecutionNodes.push_back(static_cast<IExecutionNode*>(executionGraph));
    }

private:
    static std::deque<IExecutionNode*> m_ExecutionNodes;
    static SpinLock m_ExecutionNodesLock;
};

std::deque<IExecutionNode*> ExecutionScheduler::m_ExecutionNodes = std::deque<IExecutionNode*>();
SpinLock ExecutionScheduler::m_ExecutionNodesLock = SpinLock();

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

    ExecutionScheduler::Run();
}