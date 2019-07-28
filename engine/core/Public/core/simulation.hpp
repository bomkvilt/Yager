#ifndef CORE_SIMULATION_HPP
#define CORE_SIMULATION_HPP

#include <chrono>
#include <atomic>
#include <boost/noncopyable.hpp>

#include "interface/ilifecycle.hpp"
#include "threading/thread_task.hpp"
#include "config/simulation_config.hpp"
#include "core/tick.hpp"
#include "common.hpp"



class Simulation final : public ILifecycle 
	, boost::noncopyable
{
public:
	static UNIQUE(Simulation) New();

	 Simulation();
	~Simulation();

public: //~~~~~~~~~~~~~~| << ILifecycle
	virtual void OnConstructed() override;
	virtual void OnBeginPlay() override;
	virtual void OnPause() override;
	virtual void OnResume() override;
	virtual void OnEndPlay() override;
	virtual void OnDestruction() override;

private: //~~~~~~~~~~~~~~| ticks
	using SClock = std::chrono::system_clock;
	using STime = SClock::time_point;
	
	std::tuple<threading::FTask::ptr, threading::FTask::ptr> CreateNext(ETickPhase phase);
	void AddTask(threading::FTask::ptr&& task);
	void DoTick();

private: 
	std::atomic_int ntasks = 0;

	TickManager ticks;			//!< tick manager
	STime time_current;			//!< real time point
	FReal time_simulation = 0;	//!< simulation time passed after a starting
	FReal time_delta      = 0;  //!< last frame delta step

	FSimulationConfig config;

private:

	static constexpr std::array<std::array<bool, ETickPhaseCount>, ELifeStageCount> GetTickStages();
};


#endif //!CORE_SIMULATION_HPP
