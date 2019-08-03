#ifndef CORE_WORLD_CONFIG_HPP
#define CORE_WORLD_CONFIG_HPP

#include "config/config.hpp"


struct FWorldConfig : public reflect::FConfig
{
	ARCH_BEGIN(reflect::FConfig)
		ARCH_FIELD(, , fps)
		ARCH_END()
public:

	int fps = 30;
};


#endif //!CORE_WORLD_CONFIG_HPP
