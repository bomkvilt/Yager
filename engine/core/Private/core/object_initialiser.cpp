#include "core\object_initialiser.hpp"



bool ObjectInitialiser::Validate()
{
	return world != nullptr
		&& name != ""
		&& uid != 0;
}
