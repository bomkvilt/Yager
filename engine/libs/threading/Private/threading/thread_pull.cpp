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

		// fix thread count
		if (!config.threads)
		{
			config.threads = std::thread::hardware_concurrency() - 1;
		}

		// create workers
		for (int i = 0; i < config.threads; ++i)
		{
			++nthreads;
			threads.create_thread(boost::bind(&Threadpool::Worker, this));
		}
	}

	Threadpool::~Threadpool()
	{
		bAlive = false;
		while (nthreads)
		{
			std::this_thread::yield();
		}
	}

	void Threadpool::AddTask(FTask::ptr&& task)
	{
		auto& storage = GetLocalStorage();
		storage.tasks.Push(std::move(task));
	}

	void Threadpool::Worker()
	{
		SVector localTasks;
		while (bAlive)
		{
			if (!Handle(localTasks))
			{ 
				std::this_thread::yield(); 
			}
		}
		--nthreads;
	}

	bool Threadpool::Handle(SVector& localTasks)
	{
		if (GetTasks(localTasks))
		{	/// execute
			for (auto& task : localTasks)
			{
				task->Run(*this);
				task->OnFinished();
			}
			return true;
		}
		return false;
	}

	bool Threadpool::GetTasks(SVector& localTasks)
	{
		localTasks.clear();
		
		/// lock a pool's task list
		std::scoped_lock _(tasks.tasks_mu);
		
		/// move tasks from a global storage
		auto pos = tasks.tasks.begin();
		auto end = tasks.tasks.end();
		while (pos != end)
		{	
			auto&& ptr = *pos;
			if (ptr->NPrev())
			{
				++pos;
				continue;
			}

			localTasks.push_back(std::move(ptr));
			pos = tasks.tasks.erase(pos);

			if (localTasks.size() == config.localThreadStorage)
			{
				return true;
			}
		}

		/// fiil the glbal tasks storage in
		if (localTasks.size() == 0)
		{	/// lock lockal storages globaly. | \note: it's elements are non-blocked yet
			std::scoped_lock _(localStorages_mu);
			for (auto&& [tid, ttasks] : localStorages)
			{	/// move all elements to the global list
				ttasks.tasks.Flush(tasks);
			}
		}
		return localTasks.size();
	}

	Threadpool::FLocalStorage& Threadpool::GetLocalStorage()
	{
		auto tid = threading::GetThreadID();
		auto pos = localStorages.find(tid);
		auto end = localStorages.end();
		if (pos == end)
		{
			std::scoped_lock _(localStorages_mu);
			return localStorages[tid];
		}
		return pos->second;
	}

	IThreadpool& IThreadpool::Get()
	{
		static Threadpool pool;
		return pool;
	}

	void Threadpool::SList::Push(FTask::ptr&& task)
	{
		std::scoped_lock _(tasks_mu);
		tasks.push_back(std::move(task));
	}

	void Threadpool::SList::Flush(SList& r)
	{
		std::scoped_lock _(tasks_mu);
		for (auto& task : tasks)
		{
			r.tasks.emplace_back(std::move(task));
		}
		tasks.clear();
	}
}
