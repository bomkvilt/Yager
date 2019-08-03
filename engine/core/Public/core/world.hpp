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
	UNIQUE(ThreadContext::WorldContext) worldContext;
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
	/// storage of allocated objects
	std::unordered_map<Object*, UNIQUE(Object)> createdObjects;
	std::shared_mutex createdObjects_mu;

	/// list of objects, assigned to another one
	/// \note: if owner gets destroyed all assigned objects are destroyed too.
	std::unordered_map<Object*, std::vector<Object*>> assignedObjects;
	std::shared_mutex assignedObjects_mu;

	/// objects marcked on to be destructed
	/// \note: all depended objects will be destroyed in the same order they were created
	std::unordered_set<Object*> removedObjects;
	std::vector<Object*> removedObjectsList;
	std::shared_mutex removedObjects_mu;

	/// list of objects created but not initialied bu OnBeginPLay()
	std::vector<Object*> nonStartedObjects;
	std::shared_mutex nonstartedObjects_mu;

	/// last used object unique id
	std::atomic<UID> lastUID;

public:
	/// NewObject creates a new object the world inside
	/// \note: all required params must be sent with use of ThreadContext's initialisers
	template<typename T>
	T* NewObject(ObjectInitialiser& init)
	{
		init.world = this;
		init.uid = ++lastUID;
		ThreadContext::ScopeInit _(init);
		auto ptr = std::make_unique<T>();
		RegisterObject(std::move(ptr));
		return ptr.get();
	}

	/// DelObject marks the object and all assigned once as deleted
	/// \note: it removes the objects asyncronously
	void DelObject(Object* object);

	bool IsValid(Object* object);

private:
	void RegisterObject(UNIQUE(Object)&& object);
	void RemoveObjects();
};


#endif //!CORE_WORLD_HPP
