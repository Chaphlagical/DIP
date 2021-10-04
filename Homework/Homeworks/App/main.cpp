#include <Base.h>

#include "../Homework1/HW1Layer.h"
#include "../Homework2/HW2Layer.h"

#include <random>

int main()
{
	int f = 40;
	int n = 1000000;

	Chaf::Log::Init();

	auto app = Chaf::Create();

	app->PushLayer(new Chaf::HW1Layer);
	app->PushLayer(new Chaf::HW2Layer);

	app->Run();

	return 0;
}