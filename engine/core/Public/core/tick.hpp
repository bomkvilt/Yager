#ifndef CORE_TICK_HPP
#define CORE_TICK_HPP

#include <functional>
#include <shared_mutex>
#include <boost/noncopyable.hpp>
#include "common/hit_timer.hpp"
#include "core/types.hpp"

/**/ class TickManager;
/**/ class Object;
/**/ class Actor;


/** Tick function binder
 *
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
	FTickFunction() noexcept = default;			//!< create default empty function
	FTickFunction(FTickFunction&& r) noexcept;	//!< noncopiable => move sematics
	~FTickFunction();							//!< unregister self
	
	template<typename CLB>								
	void Bind(Object* target, CLB function) noexcept	//!< bind the momber function as a tick
	{ 
		DoBind(target, std::bind(function, target)); 
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


/* Tick system.
Requirements:
- parallel execution support (per actor handlers must be provided)					-> @see FBucket
- consistency		(one actor's ticks must be executed lineary; one by one)		-> ???
- atomacy			(during execution tick functions can be changed)				-> ???
- component first	(actor component's ticks must be executed befor actor's ones)	-> @see FBucket.AddFunction()
- private/public tick phases support (manager must provide speciasised methods)		-> ???
 */
class TickManager final : boost::noncopyable
{
public:
	using SBuckets = std::map<Actor*, TickManagerUtiles::FBucket>;
	void Assign(Object& object, FTickFunction& tick);	//!< assign the tick function
	void Remove(Object& object, FTickFunction& tick);	//!< remove the tick funciton 
	const SBuckets& GetBuckets() const;

	TickManager();
	void Tick();	//!< perform internal steps and garbage collection | Note: must be called onese a circule

	void lock();
	void unlock();
	void lock_shared();
	void unlock_shared();

private:
	std::shared_mutex mu;
	SBuckets buckets;
	HitTimer hits;
};


#endif //!CORE_TICK_HPP
