#include "thread.hpp"

Thread::ID Thread::GetID()
{
	return std::this_thread::get_id();
}
