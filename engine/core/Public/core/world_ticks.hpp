#ifndef CORE_WORLD_TICKS_HPP
#define CORE_WORLD_TICKS_HPP

#include <functional>
#include <shared_mutex>
#include <boost/noncopyable.hpp>

#include "common/hit_timer.hpp"
#include "core/types.hpp"
#include "threading/thread_task.hpp"

/**/ class TickManager;
/**/ class Object;



/** Tick function binder
 */
struct FTickFunction final : boost::noncopyable
{	
	using SCallback  = std::function<void(FReal, ETickPhase, FTickFunction&)>;
private:
	TickManager* manager	= nullptr;					//!< assigned funtion manager
	ETickPhase	 phase		= ETickPhase::ePrePhysics;	//!< function call phase
	ETickType	 type		= ETickType::ePublic;		//!< function type
	SCallback    callback	= nullptr;					//!< the component's function
	bool		 bActive	= true;						//!< whether the tick must be executed
	Object*		 object		= nullptr;					//!< tick owner (component, module, actor)

public: 
	FTickFunction() noexcept;					//!< create default empty function (current world assigned)
	FTickFunction(FTickFunction&& r) noexcept;	//!< noncopiable => move sematics
	~FTickFunction() noexcept;					//!< unregister self
	
	/** Bind binds the object's method to an assigned tick manager
	 * \note: Don't use the function in a constructor
	 */
	template<typename F>								
	void Bind(Object* target, F function) noexcept
	{ 
		DoBind(target, std::bind(function, target, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); 
	}

	void Tick(FReal deltaTime, ETickPhase phase);	//!< execute the tick
	
	bool operator==(const FTickFunction& r) const;

public:
	void SetManager(TickManager* newManager);	//!< set the manager
	void SetPhase(ETickPhase newState);			//!< set the phase flag
	void SetType(ETickType newState);			//!< set the privasy flag
	void SetActive(bool newState);				//!< set the bActive flag

public:
	bool GetActive() const;					//!< get the bActive flag
	ETickPhase GetPhase() const;			//!< get the phase flag
	ETickType GetType() const;				//!< get the prvacy flag
		  TickManager* GetManager();		//!< get the manager
	const TickManager* GetManager() const;	//!< get the manager

private:
	void DoBind(Object* target, SCallback function);
	void AssignSelf();
	void RemoveSelf();
};


namespace TickManagerUtiles
{
	using SList = std::list<FTickFunction*>;	//!< container with non-invalidateable iterators

	class Iterator
	{ //!^ iterator that selects specific tick functions
	private:
		SList::iterator pos;							//!< 
		SList::iterator end;							//!< 
		ETickPhase phase = ETickPhase::ePostPhysics;	//!< selected tick phase
		ETickType  type  = ETickType::ePublic;			//!< selected tick type

	public:
		Iterator() = default;
		Iterator(SList& list, ETickPhase phase, ETickType type);

		bool operator==(const Iterator& r) const;
		bool operator!=(const Iterator& r) const;
		Iterator operator++();

		FTickFunction& operator*();
		FTickFunction* operator->();
		const FTickFunction& operator*() const;
		const FTickFunction* operator->() const;
	};


	struct FSlice
	{ //!^ lazy container with tick function search params
	private:
		SList&	   list;	//!< assigned bucket
		ETickPhase phase;	//!< selected tick phase
		ETickType  type;	//!< selected tick type

	public:
		FSlice(SList& list, ETickPhase phase, ETickType type);
		
		Iterator begin();
		Iterator end();
		const Iterator begin() const;
		const Iterator end()   const;
	};


	struct FBucket final : boost::noncopyable
	{ //!^ list of functions
	private:
		SList functions;	//!< assigned tick functions
		
	public:
		bool IsEmpty() const;
		bool IsEmpty(ETickPhase phase, ETickType type) const;
		      FSlice Slice(ETickPhase phase, ETickType type);
		const FSlice Slice(ETickPhase phase, ETickType type) const;
		void AddFunction(FTickFunction& tick);
		void RemFunction(FTickFunction& tick);
	};
}


class TickManager final : boost::noncopyable
{
private: //---| 
	HitTimer hits;

public:
	TickManager();
	void Tick();

public: //----| block.events
	using SEvent  = std::function<void(ETickPhase)>;
	using SEvents = std::array<std::vector<SEvent>, ETickPhaseCount>;

private:
	SEvents events;

	void CallPhaseEvents(ETickPhase phase);

public:
	void AddEvent(ETickPhase phase, SEvent event);
	void DelEvent(ETickPhase phase, SEvent event);

private: //---| block.ticks
	using SBuckets = std::map<Object*, TickManagerUtiles::FBucket>;

	SBuckets buckets;
	
public:
	using STaskPair = std::pair<threading::FTask::ptr, threading::FTask::ptr>;

	void Assign(Object& object, FTickFunction& tick);
	void Remove(Object& object, FTickFunction& tick);
	
	/// GetTasks returns {public, private} task backets
	/// \note: events are already injected here (public)
	STaskPair GetTasks(ETickPhase phase, FReal ds);

private: //---| internal
	std::shared_mutex mu;
};


#endif //!CORE_WORLD_TICKS_HPP
