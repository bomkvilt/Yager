#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <chrono>
#include <atomic>
#include <boost/noncopyable.hpp>
#include "interface/ilifecycle.hpp"
#include "common.hpp"


class Simulation final : public ILifecycle 
	, public boost::noncopyable
{
public:
	static UNIQUE(Simulation) New();

public: //~~~~~~~~~~~~~~| << ILifecycle
	virtual void OnConstructed() override;
	virtual void OnBeginPlay() override;
	virtual void OnPause() override;
	virtual void OnResume() override;
	virtual void OnEndPlay() override;
	virtual void OnDestruction() override;

private: //~~~~~~~~~~~~~~| time
	using SStep = std::function<void()>;
	using STime = std::chrono::system_clock::time_point;
	
	void DoNext(std::function<void()> next);
	void DoTick();

private: 
	std::atomic_bool bActive = true;
	std::atomic_int  ntasks = 0;
	SStep onTick;
};


#endif //!SIMULATION_HPP
