#include "threading/thread_pull.hpp"
#include "threading/thread_impl.hpp"


namespace threading
{
	IThreadpool& IThreadpool::Get()
	{
		static Threadpool pool;
		return pool;
	}
}
