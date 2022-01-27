#include "Core.h"

#include <iostream>

#pragma region EngineCore
class IEntity
{
public: 
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
};

class ISystem 
{
public:
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
};

class IStaticModule
{
public:
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
};
#pragma endregion

#pragma region ExampleModule
class ExampleIntegrant : public IIntegrant 
{
public:
	ExampleIntegrant()
	{
	}

	ExampleIntegrant(int integer)
		: ExampleData(integer)
	{
	}

	void DoStuff(int integer)
	{
		ExampleData += integer;
	}

public:
	int ExampleData;
};

class ExampleStaticModule : public IStaticModule
{
public:
	void Initialize() override
	{
		IntegrantRegistry::RegisterIntegrant<ExampleIntegrant>();
	}
};

class ExampleSystem : public ISystem
{
public:
	void DoStuff()
	{
		//IntegrantRegistry::EachIntegrant<ExampleIntegrant>([](ExampleIntegrant& meshRendererIntegrant) { });
	}
};

class ExampleEntity : public IEntity
{
public:
	void Initialize() override
	{
		m_ExampleIntegrantHandle = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(1234);
		m_ExampleIntegrantHandle->DoStuff(1234);
		m_ExampleIntegrantHandle->ExampleData += 1234;

		IntegrantRegistry::DestroyIntegrant<ExampleIntegrant>(m_ExampleIntegrantHandle);
	}

private:
	IntegrantHandle<ExampleIntegrant> m_ExampleIntegrantHandle;
};
#pragma endregion

int main(int argc, char* argv[])
{
	IntegrantHandle<ExampleIntegrant> exampleIntegrant = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(2);

	std::cout << exampleIntegrant->ExampleData << '\n';

	return 0;
}