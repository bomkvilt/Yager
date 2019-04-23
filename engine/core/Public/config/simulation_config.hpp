#ifndef SIMULATION_CONFIG_HPP
#define SIMULATION_CONFIG_HPP

#include "config/config.hpp"


struct FSimulationConfig : public reflect::FConfig
{
	ARCH_BEGIN(reflect::FConfig)
		ARCH_FIELD(, , fps)
		ARCH_END()

	float fps = 30;
};


#endif //!SIMULATION_CONFIG_HPP
