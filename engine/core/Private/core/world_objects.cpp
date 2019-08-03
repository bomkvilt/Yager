#include "core/world_objects.hpp"



ObjectManager::ObjectManager(World* world)
	: world(world)
	, root(nullptr)
	, lastUID(0)
{
	auto init = ObjectInitialiser();
	init.name = "root object";
	root = NewObject<Object>(init);
}

ObjectManager::~ObjectManager()
{
	if (IsValid(root))
	{
		RemAllObjects();
	}
}

void ObjectManager::InitNewObjects()
{
	std::scoped_lock _(nonstartedObjects_mu);
	for (auto* object : nonStartedObjects)
	{
		object->OnConstructed();
		object->OnBeginPlay();
	}
}

void ObjectManager::RemObject(Object* object)
{
	struct FNode : public boost::noncopyable
	{
		Object* owner;
		std::vector<Object*>::const_iterator pos;
		std::vector<Object*>::const_iterator end;

		FNode(Object* object, ObjectManager* manger)
			: owner(object)
		{
			auto& vector = manger->assignedObjects.find(object)->second;
			pos = vector.begin();
			end = vector.end();
		}
	};

	std::scoped_lock _(removedObjects_mu, assignedObjects_mu);

	auto nodes = std::stack<FNode>();
	nodes.emplace(object, this);

	while (nodes.size() > 0)
	{
		auto& top = nodes.top();
		if (top.pos == top.end)
		{
			top.owner->OnEndPlay();
			removedObjects.insert(top.owner);
			removedObjectsList.push_back(top.owner);
			assignedObjects.erase(top.owner);
			continue;
		}
		nodes.emplace(*(++top.pos), this);
	}
}

void ObjectManager::RemAllObjects()
{
	RemObject(root);
}

void ObjectManager::Valuum()
{
	std::scoped_lock _(removedObjects_mu, createdObjects_mu);

	auto pos = removedObjectsList.rbegin();
	auto end = removedObjectsList.rend();
	for (; pos != end; ++pos)
	{
		auto* object = *pos;
		object->OnDestruction();
		createdObjects.erase(object);
	}
	removedObjects.clear();
	removedObjectsList.clear();
}

bool ObjectManager::IsValid(Object* object)
{
	std::shared_lock _ (removedObjects_mu);
	std::shared_lock __(createdObjects_mu);
	return (removedObjects.find(object) == removedObjects.end())
		&& (createdObjects.find(object) != createdObjects.end());
}

void ObjectManager::RegisterObject(UNIQUE(Object)&& object)
{
	auto* ptr = object.get();
	{
		std::scoped_lock _(createdObjects_mu, assignedObjects_mu);

		createdObjects[ptr] = std::move(object);

		if (auto owner = ptr->GetOwner())
		{
			assignedObjects[owner].push_back(ptr);
		}
		else
		{
			assignedObjects[root].push_back(ptr);
		}
	}
	{
		std::scoped_lock _(nonstartedObjects_mu);
		nonStartedObjects.emplace_back(ptr);
	}
}
