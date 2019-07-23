#ifndef CORE_OBJECT_INITIALISER_HPP
#define CORE_OBJECT_INITIALISER_HPP

#include "threading/thread_context.hpp"
#include "core/core_types.hpp"


struct FObjectInitialiser : public IInitialiser
{
	FName name;
	UID   uid;
};


#endif //!CORE_OBJECT_INITIALISER_HPP
