#ifndef CORE_WORLD_OBJECT_HPP
#define CORE_WORLD_OBJECT_HPP

#include "common.hpp"
#include "core/types.hpp"


struct ITransformable
{
public:
	virtual FTransform  GetTransform(EWorldType world) const = 0;
	virtual FVector     GetLocation (EWorldType world) const = 0;
	virtual FQuat       GetRotation (EWorldType world) const = 0;

public:
	virtual FTransform& GetTransform(EWorldType world) = 0;
	virtual FVector&    GetLocation (EWorldType world) = 0;
	virtual FQuat&      GetRotation (EWorldType world) = 0;

public:
	virtual ~ITransformable() = default;
};


#endif //!CORE_WORLD_OBJECT_HPP
