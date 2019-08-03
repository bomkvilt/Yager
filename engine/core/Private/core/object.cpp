#include "core/object.hpp"
#include "core/object_initialiser.hpp"
#include "threading/thread_context.hpp"


void Object::OnConstructed()
{
	ILifecycle::OnConstructed();

	primaryTick.Bind(this, &Object::PrimaryTick);
}

void Object::OnBeginPlay()
{
	ILifecycle::OnBeginPlay();
}

void Object::OnPause()
{
	ILifecycle::OnPause();
}

void Object::OnResume()
{
	ILifecycle::OnResume();
}

void Object::OnEndPlay()
{
	ILifecycle::OnEndPlay();
}

void Object::OnDestruction()
{
	ILifecycle::OnDestruction();
}

void Object::Tick(float ds)
{
	ITicker::Tick(ds);
}


void Object::PrimaryTick(FReal ds, ETickPhase phase, FTickFunction& tick)
{
	Tick(ds);
}

Object::Object()
	: uid(0)
	, world(nullptr)
	, owner(nullptr)
{
	auto init = ThreadContext::TopInitialiser<ObjectInitialiser>();
	uid   = init.uid;
	name  = init.name;
	owner = init.owner;
	world = init.world;
}


UID	Object::GetUID() const
{
	return uid;
}

std::string	Object::GetName() const
{
	return name;
}

Object* Object::GetOwner()
{
	return owner;
}

const Object* Object::GetOwner() const
{
	return owner;
}

World* Object::GetWorld()
{
	return world;
}

const World* Object::GetWorld() const
{
	return world;
}
