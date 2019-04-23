#include "core/simulation.hpp"
#include "threading/thread_pull.hpp"
#include <iostream>


UNIQUE(Simulation) Simulation::New()
{
	return std::make_unique<Simulation>();
}

void Simulation::OnConstructed()
{	/// init time
	time_current = SClock::now();
}

void Simulation::OnBeginPlay()
{	/// start tick circule
	using namespace threading;
	IThreadpool::Get().AddTask(FLambdaTask::New([this]() { DoTick(); ++ntasks; }));
}

void Simulation::OnPause()
{}

void Simulation::OnResume()
{}

void Simulation::OnEndPlay()
{
	bActive = false;
	while (ntasks)
	{
		std::this_thread::yield();
	}
}

void Simulation::OnDestruction()
{}

std::tuple<threading::FTask::ptr, threading::FTask::ptr> Simulation::CreateNext(ETickPhase phase)
{
	using namespace threading;
	auto& buckets = ticks.GetBuckets();
	/// get private ticks
	auto privatetasks = FTasks::New();
	for (auto&& [actor, bucket] : buckets)
	{
		auto type = ETickType::ePrivate;
		if (bucket.IsEmpty(phase, type))
		{
			auto slice = bucket.Slice(phase, type);
			privatetasks->AddTask(FLambdaTask::New([slice, phase, this]()
			{
				for (auto& task : slice)
				{
					task.Tick(time_delta, phase);
				}
			}));
		}
	}
	/// get public ticks
	auto publictasks = FLambdaTask::New([&buckets, phase, this]()
	{
		auto type = ETickType::ePublic;
		for (auto&& [owner, bucket] : buckets)
		{
			auto& slice = bucket.Slice(phase, type);
			for (auto&& task : slice)
			{
				task.Tick(time_delta, phase);
			}
		}
	});
	return { std::move(publictasks), std::move(privatetasks) };
}

void Simulation::DoTick() //TODO:: substruct tick time avg error
{
	{	/// break the circule
		if (--ntasks; !bActive)
		{
			return;
		}
	}
	{	/// sleep unil the tick period end and updte time
		using namespace std::chrono_literals;
		static const auto s1 = std::chrono::duration_cast<SRealTime>(1s);

		auto time_step   = 1s / config.fps;
		auto time_before = time_current;
		{	/// time passed since a last call
			auto      time_now   = SClock::now();
			SRealTime time_spent = time_now - time_current;
			/// sleep if required
			if (time_spent < time_step)
			{
				auto sleep = time_step - time_spent;
				std::this_thread::sleep_for(sleep);
			}
		}
		{	/// update a current time
			time_current = SClock::now();
			time_delta   = std::chrono::duration_cast<SRealTime>(time_current - time_before) / 1s;
			/// update simulation time
			time_simulation += time_delta;

			std::cout << time_delta << std::endl; //TODO:: remove
		}
	}
	{	/// create tasks and lunch them
		using namespace threading;

		/// create and link tasks
		std::vector<FTask::ptr> tasks;
		for (auto phase : {
			  ETickPhase::eInputParsing
			, ETickPhase::ePrePhysics
			, ETickPhase::eInPhysics
			, ETickPhase::ePostPhysics
			, ETickPhase::eSerialisation })
		{
			auto [publicTasks, privateTasks] = CreateNext(phase);
			if (tasks.size())
			{ 
				tasks.back()->Next(publicTasks);
			}
			privateTasks->Next(publicTasks);
			tasks.emplace_back(std::move(privateTasks));
			tasks.emplace_back(std::move(publicTasks));
		}
		/// next cirule
		{
			auto tickTask = FLambdaTask::New([this]() { DoTick(); ++ntasks; });
			tasks.back()->Next(tickTask);
			tasks.emplace_back(std::move(tickTask));
		}
		/// push tasks
		for (auto& task : tasks)
		{
			IThreadpool::Get().AddTask(std::move(task));
		}
	}
}
