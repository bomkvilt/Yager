#include "threading/thread_impl.hpp"

#include <iostream>


namespace threading
{
	Threadpool::~Threadpool()
	{
		bAlive = false;
		while (nthreads)
		{
			std::this_thread::yield();
		}
	}
	
	void Threadpool::Construct(FThreadingConfig newConfig)
	{
		auto& config = globalStorage.config;

		if (bConfig)
		{
			throw std::runtime_error("Threadpull is already configured");
		}
		else globalStorage.config = newConfig;

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

	void Threadpool::AddTask(FTask::ptr&& task)
	{
		auto& storage = GetLocalStorage();
		if (storage.bWorkerThread)
		{
			storage.tasks.push(std::move(task));
			if (storage.tasks.size() > storage.config.tls_overload)
			{ 
				FlushLocalBuffer();
			}
			return;
		}
		globalStorage.tasks.enqueue(std::move(task));
	}

	void Threadpool::Worker()
	{
		GetLocalStorage().bWorkerThread = true;

		while (bAlive)
		{
			if (!Handle())
			{ 
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(3us);
			}
		}
		--nthreads;
	}

	bool Threadpool::Handle()
	{
		if (FillLocalStorage())
		{	/// execute
			auto& storage = GetLocalStorage();
			while (storage.tasks.size())
			{
				auto task = std::move(storage.tasks.front());
				storage.tasks.pop();
				task->Run(*this);
				task->OnFinished();
			}
			return true;
		}
		return false;
	}

	bool Threadpool::FillLocalStorage()
	{
		auto& storage = GetLocalStorage();
		auto size = storage.tasks.size();
		auto max  = storage.config.tls_capacity;
		
		FlushLocalBuffer();
		if (size >= max)
		{
			return true;
		}

		auto task = FTask::ptr();
		while (storage.tasks.size() < max)
		{
			if (globalStorage.tasks.try_dequeue(task))
			{
				storage.tasks.push(std::move(task));
			}
			else break;
		}
		return storage.tasks.size();
	}

	bool Threadpool::FlushLocalBuffer()
	{
		auto& storage = GetLocalStorage();
		if (storage.tasks.size() < storage.config.tls_overload)
		{
			return false;
		}

		auto max = storage.config.tls_capacity;
		while (storage.tasks.size() > max)
		{
			auto task = std::move(storage.tasks.front());
			storage.tasks.pop();
			globalStorage.tasks.enqueue(std::move(task));
		}
		return true;
	}

	Threadpool::FLocalStorage& Threadpool::GetLocalStorage()
	{
		thread_local static auto storage = FLocalStorage();
		return storage;
	}
}
