#include "threading/thread_pull.hpp"
#include "threading/thread_impl.hpp"


namespace threading
{
	void Threadpool::Construct(FThreadingConfig newConfig)
	{
		if (bConfig)
		{
			throw std::runtime_error("Threadpull is already configured");
		}
		else config = newConfig;

		if (!config.threads)
		{
			config.threads = std::thread::hardware_concurrency() - 1;
		}

		for (int i = 0; i < config.threads; ++i)
		{
			threads.create_thread(boost::bind(&Threadpool::Worker, this));
		}
	}

	void Threadpool::AddTask(FTask::ptr&& task)
	{
		std::scoped_lock(tasks_mutex);
		tasks.emplace_back(std::move(task));
	}

	void Threadpool::Worker()
	{
		static auto worker = [&]()
		{
			std::unique_lock _(tasks_mutex);
			if (!tasks.size())
			{
				_.unlock();
				std::this_thread::yield();
				return;
			}
			auto pos = tasks.begin();
			auto end = tasks.end();
			for (; pos != end; ++pos)
			{
				if (!(*pos)->prev)
				{
					auto task = std::move(*pos);
					tasks.erase(pos);
					_.unlock();

					task->Run(*this);
					task->OnFinished();
					return;
				}
			}
		};

		while (true)
		{
			worker();
		}
	}

	IThreadpool& IThreadpool::Get()
	{
		static Threadpool pool;
		return pool;
	}
}
