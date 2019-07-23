#ifndef TICK_HPP
#define TICK_HPP

#include <functional>
#include <boost/noncopyable.hpp>
#include "common/hit_timer.hpp"
#include "core/core_types.hpp"

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
	FTickFunction() noexcept = default;					//!< create default empty function
	FTickFunction(FTickFunction&& r) noexcept;			//!< noncopiable => move sematics
	~FTickFunction();									//!< unregister self
	
	template<typename CLB>								
	void Bind(Object* target, CLB function) noexcept	//!< bind the momber function as a tick
	{ DoBind(target, std::bind(function, target)); } 

	void Tick(FReal deltaTime, ETickPhase phase);		//!< execute the tick
	
	void SetManager(TickManager* newManager);			//!< set the manager
	void SetPhase(ETickPhase newState);					//!< set the phase flag
	void SetType(ETickType newState);					//!< set the privasy flag
	void SetActive(bool newState);						//!< set the bActive flag

	bool       GetActive() const;						//!< get the bActive flag
	ETickPhase GetPhase () const;						//!< get the phase flag
	ETickType  GetType  () const;						//!< get the prvacy flag
		  TickManager* GetManager();					//!< get the manager
	const TickManager* GetManager() const;				//!< get the manager

	bool operator==(const FTickFunction& r) const;

private:
	void DoBind(Object* target, SCallback function);
	void AssignSelf();
	void RemoveSelf();
};


namespace TickManagerUtiles
{
	enum class EEntery
	{
		  eActor		= 0
		, eModule		= 1
		, eComponent	= 2
	};

	using SList = std::list<FTickFunction*>;	//!< container with non-invalidateable iterators

	class Iterator
	{
	private:
		SList::iterator pos;	//!< 
		SList::iterator end;	//!< 
		ETickPhase phase;		//!< selected tick phase
		ETickType  type;		//!< selected tick type

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
	{
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


	struct FBacket final : boost::noncopyable
	{
	private:
		SList functions;	//!< assigned tick functions
		
	public:
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
	using SBuckets = std::map<Actor*, TickManagerUtiles::FBacket>;
	void Assign(Object& object, FTickFunction& tick);	//!< assign the tick function
	void Remove(Object& object, FTickFunction& tick);	//!< remove the tick funciton 
	const SBuckets& GetBuckets() const;

	TickManager();
	void Tick();	//!< perform internal steps and garbage collection | Note: must be called onese a circule

private:
	SBuckets buckets;
	HitTimer hits;
};


#endif //!TICK_HPP
