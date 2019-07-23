#ifndef CORE_SIMULATION_CONFIG_HPP
#define CORE_SIMULATION_CONFIG_HPP

#include "config/config.hpp"


struct FSimulationConfig : public reflect::FConfig
{
	ARCH_BEGIN(reflect::FConfig)
		ARCH_FIELD(, , fps)
		ARCH_END()

	float fps = 30;
};


#endif //!CORE_SIMULATION_CONFIG_HPP
