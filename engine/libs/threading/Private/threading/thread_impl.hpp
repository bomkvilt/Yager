#ifndef THREAD_IMPL_HPP
#define THREAD_IMPL_HPP

#include "threading/thread_pull.hpp"


namespace threading
{
	class Threadpool final : public IThreadpool
	{
	public:
		void Construct(FThreadingConfig newConfig);

		void AddTask(FTask::ptr&& task);

	private:
		///!< just a worker
		void Worker();

	private:
		/// << aliaces
		using SThreads = boost::thread_group;

		/// << threads
		SThreads threads;

		/// << tasks
		std::mutex tasks_mutex;
		std::list<FTask::ptr> tasks; //TODO:: cash interferention is possible. Add local thread queues

		/// << config
		FThreadingConfig config;
		std::atomic_bool bConfig = false;
	};
}


#endif //!THREAD_IMPL_HPP
