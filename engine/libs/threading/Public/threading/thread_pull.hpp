#ifndef THREAD_PULL_HPP
#define THREAD_PULL_HPP

#include "threading/thread.hpp"
#include "threading/thread_task.hpp"
#include "config/thread_config.hpp"
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <atomic>
#include <mutex>
#include <list>


namespace threading
{
	struct IThreadpool : public boost::noncopyable
	{
	public:
		virtual ~IThreadpool() {};

		static IThreadpool& Get();

		virtual void Construct(FThreadingConfig newConfig) = 0;

		virtual void AddTask(FTask::ptr&& task) = 0;
	};
}

#endif //!THREAD_PULL_HPP
