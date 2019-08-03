#ifndef CORE_TYPES_HPP
#define CORE_TYPES_HPP

#include <string>
#include <array>
#include "common.hpp"


class AActor;
class UObject;


enum class ETickPhase : Int8
{
	  eInputParsing		//!< 
	, ePrePhysics		//!< 
	, eInPhysics		//!< 
	, ePostPhysics		//!< 
	, eGameLogic		//!<
	, eSerialisation	//!<
	, ePreRender		//!<
	, eInRender			//!<
};

constexpr auto ETickPhaseCount = (UInt8)ETickPhase::eInRender + 1;

constexpr std::array<ETickPhase, ETickPhaseCount> ETickPhaseList()
{
	auto list = std::array<ETickPhase, ETickPhaseCount>();
	for (int i = 0; i < ETickPhaseCount; ++i)
	{
		list[i] = ETickPhase(i);
	}
	return list;
}


enum class ETickType : Int8
{
	  ePublic	//!< common tick type. Syncronous
	, ePrivate	//!< the tick type doesn't modyfy other actors
};


enum class EWorldType : Int8
{
	  eLocal	//!< change local offset / rotation
	, eWorld	//!< change world offset / rotation
	, eParent	//!< change offset / rotation in parent basis
};


using UID   = UInt64;
using FName = std::string;


#endif //!CORE_TYPES_HPP
