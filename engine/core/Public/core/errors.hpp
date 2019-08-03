#ifndef CORE_ERRORS_HPP
#define CORE_ERRORS_HPP

#include <stdexcept>



struct InallowedTickPhase : public std::runtime_error
{
	InallowedTickPhase() : runtime_error("the action cannot be done at the tick phase") {}
};


struct InallowedLifeStage : public std::runtime_error
{
	InallowedLifeStage() : runtime_error("the action cannot be done at the life stage") {}
};


struct NoManagerIsPresented : public std::runtime_error
{
	NoManagerIsPresented() : runtime_error("no manager is presented") {}
};

#endif //!CORE_ERRORS_HPP
