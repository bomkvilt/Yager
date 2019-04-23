#ifndef OBJECT_INITIALISER_HPP
#define OBJECT_INITIALISER_HPP

#include "threading/thread_context.hpp"
#include "core/core_types.hpp"


struct FObjectInitialiser : public IInitialiser
{
	FName name;
	UID   uid;
};


#endif //!OBJECT_INITIALISER_HPP
