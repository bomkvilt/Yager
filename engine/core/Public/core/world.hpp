#ifndef CORE_WORLD_HPP
#define CORE_WORLD_HPP

#include <array>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <boost/noncopyable.hpp>

#include "common.hpp"
#include "core/types.hpp"
#include "core/world_objects.hpp"
#include "core/world_ticks.hpp"
#include "core/object_initialiser.hpp"
#include "config/world_Config.hpp"
#include "interface/ilifecycle.hpp"
#include "threading/thread_task.hpp"
#include "threading/thread_context.hpp"

/**/ class WorldController;
/**/ class Object;



class World final
	: public ILifecycle
	, public boost::noncopyable
{
public:
	static UNIQUE(World) New();

	 World();
	~World();

public: //----| block.lifecycle
	virtual void OnConstructed() override;
	virtual void OnBeginPlay() override;
	virtual void OnPause() override;
	virtual void OnResume() override;
	virtual void OnEndPlay() override;
	virtual void OnDestruction() override;

private: //---| internal
	ThreadContext::WorldContext worldContext;
	FWorldConfig config;

	
	void AddTask(threading::FTask::ptr&& task);

private: //---| block.ticks
	using SClock = std::chrono::steady_clock;
	using STime	 = SClock::time_point;

	std::atomic_int ntasks = 0;
	
	TickManager ticks;			//!< tick manager
	STime time_current;			//!< time [?] unix time
	FReal time_fromStart = 0;	//!< time [s] from start passed at the world
	FReal time_lastStep = 0;	//!< time [s] consumed by previous step

public:
	      TickManager& GetTickManager();
	const TickManager& GetTickManager() const;

private:
	void StartTicks();
	void DoTick();
	void Sleep();
	void NextLoop();

private: //---| block.objects

	ObjectManager objects;

public:
	      ObjectManager& GetObjectManger();
	const ObjectManager& GetObjectManager() const;

private: //---| block.controllers

	std::vector<WorldController*> controllers;

public:
	template<typename T>
	T* NewController(ObjectInitialiser& init)
	{
		auto* ptr = objects.NewObject(init);
		controllers.push_back(ptr);
		return ptr;
	}
};


#endif //!CORE_WORLD_HPP
