#include "core/simulation.hpp"
#include "threading/thread_pull.hpp"
#include <iostream>


UNIQUE(Simulation) Simulation::New()
{
	return std::make_unique<Simulation>();
}

void Simulation::OnConstructed()
{
	onTick = SStep([this]() { DoTick(); --ntasks; });
}

void Simulation::OnBeginPlay()
{
	DoNext(onTick);
}

void Simulation::OnPause()
{
}

void Simulation::OnResume()
{
}

void Simulation::OnEndPlay()
{
	bActive = false;
	while (ntasks)
	{
		std::this_thread::yield();
	}
}

void Simulation::OnDestruction()
{
}

void Simulation::DoNext(std::function<void()> next)
{
	using namespace threading;
	if (bActive)
	{
		++ntasks;
		IThreadpool::Get().AddTask(FLambdaTask::New(next));
	}
}

void Simulation::DoTick()
{
	using namespace std::chrono_literals;

	std::cout << "tick" << std::endl;
	std::this_thread::sleep_for(10ms);
	
	DoNext(onTick);
}
