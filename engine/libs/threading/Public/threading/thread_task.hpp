#ifndef THREAD_TASK_HPP
#define THREAD_TASK_HPP

#include <boost/noncopyable.hpp>
#include <functional>
#include <vector>
#include <atomic>
#include <memory>


namespace threading
{
	struct IThreadpool;

	/**
	 */
	struct FTask : public boost::noncopyable
	{
		using ptr = std::unique_ptr<FTask>;
	
	public:
		virtual void Run(IThreadpool& pool) = 0;

		virtual ~FTask() = default;

		template<typename Ptr>
		void Next(Ptr& task) //!< Set a next comming task
		{
			if (task)
			{
				nexts.push_back(&*task);
				++task->prev;
			}
		}

		int Prevs() const;
		int Nexts() const;

	private:
		friend class Threadpool;

		std::vector<FTask*> nexts; //!< next comming tasks
		std::atomic_int  prev = 0; //!< previous tasks

		void OnPrevDone(); //!< a previous task has been complitted
		void OnFinished(); //!< the task has boon finished
	};

	
	/**
	 */
	struct FTasks : public FTask
	{
		using  ptr = std::unique_ptr<FTasks>;
		static FTasks::ptr New();

	public:
		virtual void Run(IThreadpool& pool) override;
		virtual void SetHook(FTask::ptr&& task); //!< set atask will be when all tasks are finished
		virtual void AddTask(FTask::ptr&& task); //!< append a task

	protected:
		FTask::ptr hook;
		std::vector<FTask::ptr> tasks;
	};


	/**
	 */
	struct FLambdaTask : public FTask
	{
		using clb = std::function<void(IThreadpool&)>;
		using ptr = std::unique_ptr<FLambdaTask>;
		static FLambdaTask::ptr New(clb run);

	public:
		FLambdaTask(clb run);

		virtual void Run(IThreadpool& pool) override;

	protected:
		std::function<void(IThreadpool&)> lambda;
	};
}


#endif //!THREAD_TASK_HPP
