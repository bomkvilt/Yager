#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

#include "reflect/archived.hpp"
#include "core/types.hpp"
#include "interface/ilifecycle.hpp"
#include "interface/itransformable.hpp"

/**/ class Actor;


class Object : public reflect::FArchived
	, public ITransformable
	, public ILifecycle
{
public:
	ARCH_BEGIN(reflect::FArchived)
		ARCH_FIELD(, , name)
		ARCH_FIELD(, , uid)
		ARCH_END()
public:
	Object();

	FName  GetName() const;		//!< returns object's name
	UID	   GetUID()  const;		//!< returns object's uid
	Actor* GetActor() const;	//!< returns object's actor

private:
	Actor* actor;	//!< assigned actor
	FName  name;	//!< object name
	UID	   uid;		//!< unique object id
};


#endif //!CORE_OBJECT_HPP
