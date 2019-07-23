#ifndef CORE_ILIFECYCLE_HPP
#define CORE_ILIFECYCLE_HPP


struct ILifecycle
{	//!^ lifecycle callbacks interface
public:
	/** Lifecycle callbacks
	 * \Note: the following functions are plased in a lyfecycle sequence
	 */
	
	virtual void OnConstructed() {}	//!< on object contructed
	virtual void OnBeginPlay() {}	//!< on obejct started
	virtual void OnPause() {}		//!< on object paused
	virtual void OnResume() {}		//!< on object resumed
	virtual void OnEndPlay() {}		//!< on object play ended
	virtual void OnDestruction() {}	//!< on object resuruction begining

public:
	virtual ~ILifecycle() = default;
};


#endif //!CORE_ILIFECYCLE_HPP
