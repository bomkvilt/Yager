#include "core/simulation.hpp"
#include "threading/thread_pull.hpp"
#include <iostream>


UNIQUE(Simulation) Simulation::New()
{
	return std::make_unique<Simulation>();
}

Simulation::Simulation()
{}

Simulation::~Simulation()
{
	if (GetLifeStage() != ELifeStage::eInDestroy)
	{
		std::cerr << "still active simlation gets gestroyed" << std::endl;
	}
}

void Simulation::OnConstructed()
{	/// init time
	ILifecycle::OnConstructed();

	time_current = SClock::now();
}

void Simulation::OnBeginPlay()
{	/// start tick circule
	ILifecycle::OnBeginPlay();

	using namespace threading;
	AddTask(FLambdaTask::New([this]() { DoTick(); }));
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
	
	while (ntasks)
	{
		std::this_thread::yield();
	}
}

void Simulation::OnDestruction()
{
	ILifecycle::OnDestruction();
}

std::tuple<threading::FTask::ptr, threading::FTask::ptr> Simulation::CreateNext(ETickPhase phase)
{
	using namespace threading;
	std::shared_lock _(ticks);
	auto& buckets = ticks.GetBuckets();
	/// get private ticks
	auto privatetasks = FTasks::New();
	for (auto&& [actor, bucket] : buckets)
	{
		auto type = ETickType::ePrivate;
		if (!bucket.IsEmpty(phase, type))
		{
			auto slice = bucket.Slice(phase, type);
			privatetasks->AddTask(FLambdaTask::New([slice, phase, this]()
			{	//TODO:: add a check that only actor can register his ticks
				for (auto&& task : slice)
				{
					task.Tick(time_delta, phase);
				}
			}));
		}
	}
	/// get public ticks
	auto publictasks = FLambdaTask::New([phase, this]()
	{
		std::shared_lock _(ticks);
		auto& buckets = ticks.GetBuckets();
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

void Simulation::AddTask(threading::FTask::ptr&& task)
{
	using namespace threading;
	task->SetOnDone([this]() { --ntasks; });
	IThreadpool::Get().AddTask(std::move(task));
	++ntasks;
}

void Simulation::DoTick() //TODO:: substruct tick time avg error
{
	if (   GetLifeStage() == ELifeStage::eStoped
		|| GetLifeStage() == ELifeStage::eInDestroy )
	{	/// terminate the simulation
		return;
	}
	{	/// sleep unil the tick period end and updte time
		using namespace std::chrono_literals;

		auto time_step   = 1s / config.fps;
		auto time_before = time_current;
		{	/// time passed since a last call
			auto time_now   = SClock::now();
			auto time_spent = time_now - time_current;
			/// sleep if required
			if (time_spent < time_step)
			{
				auto sleep = time_step - time_spent;
				std::this_thread::sleep_for(sleep);
			}
		}
		{	/// update a current time
			time_current = SClock::now();
			time_delta   = std::chrono::duration_cast<std::chrono::microseconds>(time_current - time_before).count() / 1e6f;
			/// update simulation time
			time_simulation += time_delta;

			std::cout << time_delta << std::endl; //TODO:: remove
		}
	}
	{	/// create tasks and lunch them
		using namespace threading;

		static const auto tickStages = GetTickStages();
		static const auto tickPhases = ETickPhaseList();

		/// create and link tasks
		std::vector<FTask::ptr> tasks;
		for (auto phase : tickPhases)
		{
			if (!tickStages[(UInt8)GetLifeStage()][(UInt8)phase])
			{
				continue;
			}

			auto [publicTasks, privateTasks] = CreateNext(phase);
			if (tasks.size())
			{ 
				tasks.back()->Next(publicTasks);
			}
			publicTasks->Next(privateTasks);
			tasks.emplace_back(std::move(publicTasks));
			tasks.emplace_back(std::move(privateTasks));
		}
		/// next cirule
		{
			auto tickTask = FLambdaTask::New([this]() { DoTick(); });
			if (tasks.size())
			{
				tasks.back()->Next(tickTask);
			}
			tasks.emplace_back(std::move(tickTask));
		}
		/// push tasks
		for (auto& task : tasks)
		{
			AddTask(std::move(task));
		}
	}
}

constexpr std::array<std::array<bool, ETickPhaseCount>, ELifeStageCount> Simulation::GetTickStages()
{
	auto table = std::array<std::array<bool, ETickPhaseCount>, ELifeStageCount>{{0}};
	auto allow = [&](ELifeStage stage, ETickPhase phase)
	{
		table[UInt8(stage)][UInt8(phase)] = true;
	};
	auto allowAll = [&](ELifeStage stage)
	{
		for (UInt8 i = 0; i < ETickPhaseCount; ++i)
		{
			allow(stage, ETickPhase(i));
		}
	};
	allowAll(ELifeStage::eActive);
	allow(ELifeStage::ePaused, ETickPhase::eInputParsing);
	allow(ELifeStage::ePaused, ETickPhase::eSerialisation);
	allow(ELifeStage::ePaused, ETickPhase::ePreRender);
	allow(ELifeStage::ePaused, ETickPhase::eInRender);
	return table;
}
