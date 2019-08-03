#ifndef CORE_SIMULATION_HPP
#define CORE_SIMULATION_HPP

#include <boost/noncopyable.hpp>

#include "common.hpp"
#include "interface/ilifecycle.hpp"



class Simulation final
	: public ILifecycle
	, public boost::noncopyable
{
public:
	static UNIQUE(Simulation) New();

	 Simulation();
	~Simulation();

public: //----| << ILifecycle
	virtual void OnConstructed() override;
	virtual void OnBeginPlay() override;
	virtual void OnPause() override;
	virtual void OnResume() override;
	virtual void OnEndPlay() override;
	virtual void OnDestruction() override;

private: //---| internal
};


#endif //!CORE_SIMULATION_HPP
