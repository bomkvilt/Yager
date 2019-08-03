#ifndef CORE_WORLD_OBJECTS_HPP
#define CORE_WORLD_OBJECTS_HPP

#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <boost/noncopyable.hpp>

#include "core/object.hpp"
#include "core/object_initialiser.hpp"
#include "common.hpp"

/**/ class World;



class ObjectManager final : public boost::noncopyable
{
public:
	 ObjectManager(World* world);
	~ObjectManager();

public:
	/// NewObject creates a new object
	template<typename T>
	T* NewObject(ObjectInitialiser& init)
	{
		init.world = world;
		init.uid = ++lastUID;
		ThreadContext::ScopeInit _(init);
		auto ptr = std::make_unique<T>();
		RegisterObject(std::move(ptr));
		return ptr.get();
	}

	/// IniNewObjects inits all non initialised objects
	/// \note: OnConstructed()
	/// \note: OnBeginPlay()
	void InitNewObjects();

	/// RemObject marks the object and all assigned once as deleted
	/// \note: it removes the objects asyncronously
	void RemObject(Object* object);
	
	/// RemAllObjects marks all created objects as deletes and makes valuum
	void RemAllObjects();

	/// Valuum delets all objecs marcked marcked as deleted
	void Valuum();

	/// checks if the object exists and not marcked as delted
	bool IsValid(Object* object);

private://---| internal
	void RegisterObject(UNIQUE(Object)&& object);

private: 
	/// storage of allocated objects
	std::unordered_map<Object*, UNIQUE(Object)> createdObjects;
	std::shared_mutex createdObjects_mu;

	/// list of objects, assigned to another one
	/// \note: if owner gets destroyed all assigned objects are destroyed too.
	std::unordered_map<Object*, std::vector<Object*>> assignedObjects;
	std::shared_mutex assignedObjects_mu;

	/// objects marcked on to be destructed
	/// \note: all depended objects will be destroyed in the same order they were created
	std::unordered_set<Object*> removedObjects;
	std::vector<Object*> removedObjectsList;
	std::shared_mutex removedObjects_mu;

	/// list of objects created but not initialied bu OnBeginPLay()
	std::vector<Object*> nonStartedObjects;
	std::shared_mutex nonstartedObjects_mu;

	/// last used object unique id
	std::atomic<UID> lastUID;

	World* world;
	Object* root;
};



#endif //!CORE_WORLD_OBJECTS_HPP
