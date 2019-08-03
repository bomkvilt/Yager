#ifndef THREADING_THREAD_CONFIG_HPP
#define THREADING_THREAD_CONFIG_HPP

#include "config/config.hpp"



struct FThreadingConfig : public reflect::FConfig
{
	ARCH_BEGIN(reflect::FConfig)
	 	ARCH_FIELD(, , threads)
	 	ARCH_END();

	int threads = 0;		//!< threads in threadpull | \note: 0 -> (num_of_cores - 1)
	int tls_capacity = 10;	//!< 
	int tls_overload = 30;	//!< 
};


#endif //!THREADING_THREAD_CONFIG_HPP
