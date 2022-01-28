#include "Core.h"

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

int main(int argc, char* argv[])
{
	IntegrantRegistry::RegisterIntegrant<ExampleIntegrant>();

	IntegrantHandle<ExampleIntegrant> exampleIntegrant1 = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(0);
	IntegrantHandle<ExampleIntegrant> exampleIntegrant2 = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(1);
	IntegrantHandle<ExampleIntegrant> exampleIntegrant3 = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(2);
	IntegrantHandle<ExampleIntegrant> exampleIntegrant4 = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(3);
	IntegrantHandle<ExampleIntegrant> exampleIntegrant5 = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(4);

	std::cout << exampleIntegrant1->ExampleData << '\n';
	std::cout << exampleIntegrant2->ExampleData << '\n';
	std::cout << exampleIntegrant3->ExampleData << '\n';
	std::cout << exampleIntegrant4->ExampleData << '\n';
	std::cout << exampleIntegrant5->ExampleData << '\n';

	IntegrantRegistry::DestroyIntegrant(exampleIntegrant3);

	std::cout << '\n';
	std::cout << exampleIntegrant1->ExampleData << '\n';
	std::cout << exampleIntegrant2->ExampleData << '\n';
	// std::cout << exampleIntegrant3->ExampleData << '\n';
	std::cout << exampleIntegrant4->ExampleData << '\n';
	std::cout << exampleIntegrant5->ExampleData << '\n';

	return 0;
}