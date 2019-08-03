#include "core/simulation.hpp"
#include "core/world.hpp"
#include "threading/thread_pull.hpp"



int main()
{
	// init threadpull
	auto conf = FThreadingConfig();
	conf.threads = 8;
	threading::IThreadpool::Get().Construct(conf);

	// simulaition mock
	auto sim = World::New();
	sim->OnConstructed();
	sim->OnBeginPlay();
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10s);
	}
	sim->OnEndPlay();
	sim->OnDestruction();
}
