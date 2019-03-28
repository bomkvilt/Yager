#ifndef THREAD_CONFIG_HPP
#define THREAD_CONFIG_HPP

#include "config/config.hpp"


struct FThreadingConfig : public reflect::FConfig
{
	ARCH_BEGIN(reflect::FConfig)
		ARCH_FIELD(, , threads)
		ARCH_END();

	int threads = 0; //!< threads in threadpull | \note: 0 -> (cores-1)
};


#endif //!THREAD_CONFIG_HPP
