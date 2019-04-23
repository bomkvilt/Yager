#include "core/object.hpp"
#include "threading/thread_context.hpp"
#include "core/object_initialiser.hpp"


Object::Object()
{
	auto init = ThreadContext::TopInitialiser<FObjectInitialiser>();
	name = init.name;
	uid  = init.uid;
}

FName Object::GetName() const
{ return name; }

UID Object::GetUID() const
{ return uid; }

Actor* Object::GetActor() const
{ return actor; }

