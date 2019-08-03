#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

#include <boost/noncopyable.hpp>

#include "core/types.hpp"
#include "core/world_ticks.hpp"
#include "reflect/reflect.hpp"
#include "interface/ilifecycle.hpp"
#include "interface/iTicker.hpp"

/**/ class World;



/** Object is a base game elemet.
 *	\note each object is owned by a world: creates and destroys
 *	\note each object can only be created by a world and also will be killed with it.
 *	\note each object can be assigned to another object and will be destroys with it too.
 */
class Object : public reflect::FArchived
	, public ILifecycle
	, public ITicker
	, public boost::noncopyable
{
public:
	struct SerialisationFLags
	{
		Int8 bNet : 1;

		SerialisationFLags()
			: bNet(true)
		{}
	};

public: //----| block.Lifecycle
	virtual void OnConstructed() override;	//!< the object and it's owner are constructed
	virtual void OnBeginPlay() override;	//!< the object was placed to a geme scene
	virtual void OnPause() override;		//!< 
	virtual void OnResume() override;		//!<
	virtual void OnEndPlay() override;		//!<
	virtual void OnDestruction() override;	//!<

public: //----| block.ticks
	virtual void Tick(float ds) override;

protected:
	FTickFunction primaryTick;

private:
	void PrimaryTick(FReal ds, ETickPhase phase, FTickFunction& tick);

public: //----| block.object
	Object();

	SerialisationFLags serialisationFlags;

public:
	      Object* GetOwner();
	const Object* GetOwner() const;

	      World* GetWorld();
	const World* GetWorld() const;

	UID	        GetUID() const;
	std::string	GetName() const;

private:
	UID	uid;			//!< worldwide uid
	World* world;		//!< object's world
	Object* owner;		//!< object's owner
	std::string name;	//!< object's name
};


#endif //!CORE_OBJECT_HPP
