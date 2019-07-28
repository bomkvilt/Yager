#include "threading/thread_context.hpp"
#include <unordered_map>
#include "threading/thread.hpp"


FEngineConfig ThreadContext::config;


void ThreadContext::SetWorld(World* newWorld)
{
	Get().world = newWorld;
}

World* ThreadContext::GetWorld()
{
	return Get().world;
}

FEngineConfig& ThreadContext::GetConfig()
{
	return config;
}

void ThreadContext::SetConfig(FEngineConfig newConfig)
{
	config = newConfig;
}

void ThreadContext::PushInitialiser(IInitialiser& init)
{
	Get().inits.push(&init);
}

void ThreadContext::PopInitialiser()
{
	Get().inits.pop();
}

ThreadContext& ThreadContext::Get()
{
	using  Contexts = std::unordered_map<threading::ID, ThreadContext>;
	static Contexts contexts;
	return contexts[threading::GetThreadID()];
}

