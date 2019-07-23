#ifndef THREADING_THREAD_IMPL_HPP
#define THREADING_THREAD_IMPL_HPP

#include "threading/thread_pull.hpp"


namespace threading
{
	class Threadpool final : public IThreadpool
	{
	public:
		void Construct(FThreadingConfig newConfig);
		
		~Threadpool()
		{
			bAlive = false;
			while (nthreads)
			{
				std::this_thread::yield();
			}
		}

		void AddTask(FTask::ptr&& task);

	private:
		/// << types
		using SThreads	= boost::thread_group;
		using SVector	= std::vector<FTask::ptr>;

		struct SList : public boost::noncopyable
		{
			std::list<FTask::ptr>	tasks;
			std::mutex				tasks_mu;

			void Push(FTask::ptr&& task);
			void Flush(SList& r);
		};

		enum { eAllign = 256 };
		// enum { eAllign = std::hardware_destructive_interference_size };
		struct alignas(eAllign) FLocalStorage : public boost::noncopyable
		{
			SList tasks;
		};

		using SLocalMap = std::map<threading::ID, FLocalStorage>;

		/// << threads
		SThreads   threads;
		SLocalMap  localStorages;
		std::mutex localStorages_mu;
		std::atomic_int nthreads = 0;

		/// << tasks
		SList tasks;

		/// << config
		FThreadingConfig config;
		std::atomic_bool bConfig = false;
		std::atomic_bool bAlive  = true;

	private:
		void Worker();
		bool Handle  (SVector& localTasks);
		bool GetTasks(SVector& localTasks);
		FLocalStorage& GetLocalStorage();
	};
}


#endif //!THREADING_THREAD_IMPL_HPP
