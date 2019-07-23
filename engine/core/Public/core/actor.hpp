#ifndef CORE_ACTOR_HPP
#define CORE_ACTOR_HPP

#include "core/object.hpp"
#include "interface/world_object.hpp"


class Actor : public Object
	, public IWorldObject
{
public:
	Actor();

public:

};


#endif //!CORE_ACTOR_HPP
