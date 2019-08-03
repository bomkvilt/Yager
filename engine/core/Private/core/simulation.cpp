#include "core/simulation.hpp"

UNIQUE(Simulation) Simulation::New()
{
	return UNIQUE(Simulation)();
}

Simulation::Simulation()
{}

Simulation::~Simulation()
{}


void Simulation::OnConstructed()
{
	ILifecycle::OnConstructed();
}

void Simulation::OnBeginPlay()
{
	ILifecycle::OnBeginPlay();
}

void Simulation::OnPause()
{
	ILifecycle::OnPause();
}

void Simulation::OnResume()
{
	ILifecycle::OnResume();
}

void Simulation::OnEndPlay()
{
	ILifecycle::OnEndPlay();
}

void Simulation::OnDestruction()
{
	ILifecycle::OnDestruction();
}
