#include "threading/thread_context.hpp"
#include "threading/thread.hpp"
#include <unordered_map>
#include <iostream>



void ThreadContext::SetWorldContext(WorldContext* newWorld)
{
	auto& context = Get();
	if (context.inits.size())
	{	/// non-empty initialiser list means that 
		/// a previous tick handler released not all it's resources
		std::cerr << "non empty initialiser list detected" << std::endl;
	}
	Get().world = newWorld;
}

World* ThreadContext::GetWorld()
{
	return Get().world->world;
}

void ThreadContext::PushInitialiser(IInitialiser& init)
{
	Get().inits.push(&init);
}

void ThreadContext::PopInitialiser()
{
	Get().inits.pop();
}

size_t ThreadContext::CountOfInitialisers()
{
	return Get().inits.size();
}

ThreadContext& ThreadContext::Get()
{
	thread_local static ThreadContext context;
	return context;
}
