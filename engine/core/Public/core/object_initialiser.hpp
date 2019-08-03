#ifndef CORE_OBJECT_INITIALISER_HPP
#define CORE_OBJECT_INITIALISER_HPP

#include "core/types.hpp"
#include "threading/thread_context.hpp"

/**/ class World;
/**/ class Object;



class ObjectInitialiser : public IInitialiser
{
public:
	World* world;		//!< autoset
	Object* owner;		//!< required
	std::string name;	//!< required
	UID uid;			//!< autoset

	virtual bool Validate() override;
};


#endif //!CORE_OBJECT_INITIALISER_HPP
