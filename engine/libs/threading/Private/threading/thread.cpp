#include "threading/thread.hpp"


threading::ID threading::GetThreadID()
{
	return std::this_thread::get_id();
}
