#include "core/object.hpp"
#include "core/world.hpp"
#include "core/world_ticks.hpp"
#include "core/errors.hpp"
#include "threading/thread_context.hpp"
#include <algorithm>



FTickFunction::FTickFunction() noexcept
	: bActive(false)
	, manager(&ThreadContext::GetWorld()->GetTickManager())
	, object(nullptr)
	, phase(ETickPhase::eGameLogic)
	, type(ETickType::ePublic)
{ AssignSelf(); }

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
		if (object->GetLifeStage() != ELifeStage::eConstructed)
		{
			throw InallowedLifeStage();
		}
		RemoveSelf();
		callback = function;
		object = target;
		AssignSelf();
	}
	else throw NoManagerIsPresented();
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




bool TickManagerUtiles::FBucket::IsEmpty() const
{
	return functions.size();
}

bool TickManagerUtiles::FBucket::IsEmpty(ETickPhase phase, ETickType type) const
{
	for (auto&& function : functions)
	{
		if (   function->GetPhase() == phase
			&& function->GetType() == type
			&& function->GetActive())
		{
			return false;
		}
	}
	return true;
}

TickManagerUtiles::FSlice TickManagerUtiles::FBucket::Slice(ETickPhase phase, ETickType type)
{ 
	return FSlice(functions, phase, type); 
}

const TickManagerUtiles::FSlice TickManagerUtiles::FBucket::Slice(ETickPhase phase, ETickType type) const
{  
	return FSlice(*const_cast<TickManagerUtiles::SList*>(&functions), phase, type); 
}

void TickManagerUtiles::FBucket::AddFunction(FTickFunction& tick)
{ 
	functions.emplace_back(&tick); 
}

void TickManagerUtiles::FBucket::RemFunction(FTickFunction& tick)
{
	functions.remove_if([&tick](const FTickFunction* l) { return *l == tick; });
}




TickManager::TickManager()
	: hits(600)
{}

void TickManager::AddEvent(ETickPhase phase, SEvent event)
{
	events[(UInt8)phase].push_back(event);
}

void TickManager::DelEvent(ETickPhase phase, SEvent event)
{
	auto& list = events[(UInt8)phase];
	auto pos = list.begin();
	auto end = list.end();
	for (; pos != end; ++pos)
	{
		if (pos->target<void*>() != event.target<void*>())
		{
			continue;
		}
		list.erase(pos);
		return;
	}
}

void TickManager::CallPhaseEvents(ETickPhase phase)
{
	for (auto& event : events[(UInt8)phase])
	{
		event(phase);
	}
}

void TickManager::Assign(Object& object, FTickFunction& tick)
{
	if (auto* owner = object.GetOwner())
	{
		std::shared_lock _(mu);
		buckets[owner].AddFunction(tick);
	}
}

void TickManager::Remove(Object& object, FTickFunction& tick)
{
	if (auto* owner = object.GetOwner())
	{
		std::shared_lock _(mu);
		auto pos = buckets.find(owner);
		auto end = buckets.end();
		if (pos == end)
		{
			return;
		}

		auto& bucket = pos->second;
		bucket.RemFunction(tick);
	}
}

TickManager::STaskPair TickManager::GetTasks(ETickPhase phase, FReal ds)
{
	using namespace threading;
	std::shared_lock _(mu);
	
	/// get private ticks
	auto privatetasks = FTasks::New();
	for (auto&& [actor, bucket] : buckets)
	{
		auto type = ETickType::ePrivate;
		if (bucket.IsEmpty(phase, type))
		{
			continue;
		}

		auto slice = bucket.Slice(phase, type);
		privatetasks->AddTask(FLambdaTask::New([slice, ds, phase]()
		{
			for (auto&& task : slice)
			{
				task.Tick(ds, phase);
			}
		}));
	}

	/// get public ticks
	auto publictasks = FLambdaTask::New([ds, phase, this]()
	{
		CallPhaseEvents(phase);
		std::shared_lock _(mu);
		auto type = ETickType::ePublic;
		for (auto&& [owner, bucket] : buckets)
		{
			auto& slice = bucket.Slice(phase, type);
			for (auto&& task : slice)
			{
				task.Tick(ds, phase);
			}
		}
	});
	return { std::move(publictasks), std::move(privatetasks) };
}

void TickManager::Tick()
{
	if (hits.Hit())
	{ //!^ it's time to vacuum the tick table
		std::scoped_lock _(mu);
		for (auto&& [owner, bucket] : buckets)
		{
			if (!bucket.IsEmpty())
			{
				continue;
			}
			buckets.erase(owner);
		}
	}
}
