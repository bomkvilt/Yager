#ifndef CORE_ILIFECYCLE_HPP
#define CORE_ILIFECYCLE_HPP

#include "common.hpp"
#include <atomic>
#include <array>


enum class ELifeStage: UInt8
{
	  eNotInitialised
	, eConstructed
	, eActive
	, ePaused
	, eStoped
	, eInDestroy
};

constexpr auto ELifeStageCount = (UInt8)ELifeStage::eInDestroy + 1;


struct ILifecycle
{	//!^ lifecycle callbacks interface
public:
	/** Lifecycle callbacks
	 * \Note: the following functions are plased in a lyfecycle sequence
	 */
	
	virtual void OnConstructed() { SetLifeStage(ELifeStage::eConstructed); }
	virtual void OnBeginPlay()	 { SetLifeStage(ELifeStage::eActive); }
	virtual void OnPause()		 { SetLifeStage(ELifeStage::ePaused); }
	virtual void OnResume()		 { SetLifeStage(ELifeStage::eActive); }
	virtual void OnEndPlay()	 { SetLifeStage(ELifeStage::eStoped); }
	virtual void OnDestruction() { SetLifeStage(ELifeStage::eInDestroy); }
	
	virtual ~ILifecycle() throw();

	ELifeStage GetLifeStage() const;

private:
	std::atomic<ELifeStage> stage = ELifeStage::eNotInitialised;

	void SetLifeStage(ELifeStage newStage);
};


#endif //!CORE_ILIFECYCLE_HPP
