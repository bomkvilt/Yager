#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "core/object.hpp"
#include "interface/world_object.hpp"


class Actor : public Object
	, public IWorldObject
{
public:
	Actor();

public:

};


#endif //!ACTOR_HPP
