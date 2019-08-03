#ifndef THREADING_THREAD_IMPL_HPP
#define THREADING_THREAD_IMPL_HPP

#include <vector>
#include <deque>
#include <queue>
#include "threading/thread_pull.hpp"
#include "concurrentqueue/concurrentqueue.h"



namespace threading
{
	class Threadpool final : public IThreadpool
	{
	public:
		~Threadpool();

		void Construct(FThreadingConfig newConfig);
		void AddTask(FTask::ptr&& task);

	private:
		using SThreads = boost::thread_group;
		using SVector  = std::vector<FTask::ptr>;

		struct FLocalStorage : public boost::noncopyable
		{
			bool bWorkerThread = false;
			FThreadingConfig config;

			std::queue<FTask::ptr> tasks;
		};

		struct FGlobalStorage : public boost::noncopyable
		{
			FThreadingConfig config;

			moodycamel::ConcurrentQueue<FTask::ptr> tasks;
		};

		SThreads threads;
		std::atomic_int nthreads = 0;

		FGlobalStorage globalStorage;

		std::atomic_bool bConfig = false;
		std::atomic_bool bAlive  = true;

	private:
		void Worker();
		bool Handle();
		bool FillLocalStorage();
		bool FlushLocalBuffer();
		FLocalStorage& GetLocalStorage();
	};
}


#endif //!THREADING_THREAD_IMPL_HPP
