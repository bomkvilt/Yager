#include "core/simulation.hpp"
#include "threading/thread_pull.hpp"


int main()
{
	// init threadpull
	auto conf = FThreadingConfig();
	conf.threads = 8;
	threading::IThreadpool::Get().Construct(conf);

	// simulaition mock
	auto sim = Simulation::New();
	sim->OnConstructed();
	sim->OnBeginPlay();
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
	sim->OnEndPlay();
	sim->OnDestruction();
}
