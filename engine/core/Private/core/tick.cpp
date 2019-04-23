#include "core/tick.hpp"
#include "core/actor.hpp"
#include "core/object.hpp"


FTickFunction::FTickFunction(FTickFunction&& r) noexcept
	: callback(r.callback)
	, bActive(r.bActive)
	, manager(r.manager)
	, object(r.object)
	, phase(r.phase)
	, type(r.type)
{ AssignSelf(); }

FTickFunction::~FTickFunction()
{ RemoveSelf(); }

void FTickFunction::Tick(FReal deltaTime, ETickPhase phase)
{
	if (callback && bActive)
	{
		callback(deltaTime, phase, *this);
	}
}

void FTickFunction::SetActive(bool newState)
{ bActive = newState; }

void FTickFunction::SetManager(TickManager* newManager)
{
	if (manager != newManager)
	{
		RemoveSelf();
		manager = newManager;
		AssignSelf();
	}
}

void FTickFunction::SetPhase(ETickPhase newState)
{ phase = newState; }

void FTickFunction::SetType(ETickType newState)
{ type = newState; }

bool FTickFunction::GetActive() const
{ return bActive; }

ETickPhase FTickFunction::GetPhase() const
{ return phase; }

ETickType FTickFunction::GetType() const
{ return type; }

TickManager* FTickFunction::GetManager()
{ return manager; }

const TickManager* FTickFunction::GetManager() const
{ return manager; }

bool FTickFunction::operator==(const FTickFunction& r) const
{
	auto a =   callback.target<void*>();
	auto b = r.callback.target<void*>();
	return a == b;
}

void FTickFunction::DoBind(Object* target, SCallback function)
{
	if (manager)
	{
		RemoveSelf();
		callback = function;
		object   = target;
		AssignSelf();
	}
	else throw std::runtime_error("empty manager is not allowed");
}

void FTickFunction::AssignSelf()
{
	if (manager && object)
	{
		manager->Assign(*object, *this);
	}
}

void FTickFunction::RemoveSelf()
{
	if (manager && object)
	{
		manager->Remove(*object, *this);
	}
}




TickManagerUtiles::Iterator::Iterator(SList& list, ETickPhase phase, ETickType type)
	: pos(list.begin())
	, end(list.end())
	, phase(phase)
	, type(type)
{}

bool TickManagerUtiles::Iterator::operator==(const Iterator& r) const
{ return pos == r.pos; }

bool TickManagerUtiles::Iterator::operator!=(const Iterator& r) const
{ return !(*this == r); }

TickManagerUtiles::Iterator TickManagerUtiles::Iterator::operator++()
{
	for (; pos != end; ++pos)
	{
		if (   (*pos)->GetPhase() == phase
			&& (*pos)->GetType() == type
			&& (*pos)->GetActive())
		{ 
			return *this;
		}
	}
	return *this;
}

FTickFunction& TickManagerUtiles::Iterator::operator*()
{ return **pos; }

FTickFunction* TickManagerUtiles::Iterator::operator->()
{ return *pos.operator->(); }

const FTickFunction& TickManagerUtiles::Iterator::operator*() const
{ return **pos; }

const FTickFunction* TickManagerUtiles::Iterator::operator->() const
{ return *pos.operator->(); }




TickManagerUtiles::FSlice::FSlice(SList& list, ETickPhase phase, ETickType type)
	: phase(phase)
	, list(list)
	, type(type)
{}

TickManagerUtiles::Iterator TickManagerUtiles::FSlice::begin()
{ return Iterator(list, phase, type); }

TickManagerUtiles::Iterator TickManagerUtiles::FSlice::end()
{ return Iterator(); }

const TickManagerUtiles::Iterator TickManagerUtiles::FSlice::begin() const
{ return Iterator(list, phase, type); }

const TickManagerUtiles::Iterator TickManagerUtiles::FSlice::end() const
{ return Iterator(); }




bool TickManagerUtiles::FBacket::IsEmpty(ETickPhase phase, ETickType type) const
{
	for (auto&& function : functions)
	{
		if (   function->GetPhase() == phase
			&& function->GetType() == type
			&& function->GetActive())
		{
			return true;
		}
	}
	return this;
}

TickManagerUtiles::FSlice TickManagerUtiles::FBacket::Slice(ETickPhase phase, ETickType type)
{ return FSlice(functions, phase, type); }

const TickManagerUtiles::FSlice TickManagerUtiles::FBacket::Slice(ETickPhase phase, ETickType type) const
{ 
	return FSlice(*const_cast<TickManagerUtiles::SList*>(&functions), phase, type); 
}

void TickManagerUtiles::FBacket::AddFunction(FTickFunction& tick)
{ functions.emplace_back(&tick); }

void TickManagerUtiles::FBacket::RemFunction(FTickFunction& tick)
{
	functions.remove_if([&tick](const FTickFunction* l) { return *l == tick; });
}




void TickManager::Assign(Object& object, FTickFunction& tick)
{
	if (auto* actor = object.GetActor())
	{
		buckets[actor].AddFunction(tick);
	}
}

void TickManager::Remove(Object& object, FTickFunction& tick)
{
	if (auto* actor = object.GetActor())
	{
		auto pos = buckets.find(actor);
		auto end = buckets.end();
		if (pos == end)
		{
			return;
		}

		auto& bucket = pos->second;
		bucket.RemFunction(tick);
	}
}

const TickManager::SBuckets& TickManager::GetBuckets() const
{ return buckets; }
