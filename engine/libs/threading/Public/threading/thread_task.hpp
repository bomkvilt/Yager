#ifndef THREADING_THREAD_TASK_HPP
#define THREADING_THREAD_TASK_HPP

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
		using event = std::function<void()>;
	
	public:
		virtual void Run(IThreadpool& pool) noexcept;

		virtual ~FTask() = default;

		template<typename Ptr>
		void Next(Ptr& task) noexcept //!< Set a next comming task
		{
			if (task)
			{
				nexts.push_back(&*task);
				++task->prev;
			}
		}

		int NPrev() const noexcept;
		int NNext() const noexcept;

		void SetOnBegin(event event) noexcept;
		void SetOnDone (event event) noexcept;
		
	private:
		friend class Threadpool;

		std::vector<FTask*> nexts;	//!< next comming tasks
		std::atomic_int prev = 0;	//!< previous tasks
		event onBegin;				//!< on task started event
		event onDone;				//!< on task done event

		void OnPrevDone() noexcept; //!< a previous task has been complitted
		void OnFinished() noexcept; //!< the task has boon finished
	};

	
	/**
	 */
	struct FTasks : public FTask
	{
		using  ptr = std::unique_ptr<FTasks>;
		static FTasks::ptr New();

	public:
		virtual void Run(IThreadpool& pool) noexcept override;
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
		using ptr = std::unique_ptr<FLambdaTask>;
		static FLambdaTask::ptr New(std::function<void(IThreadpool&)> run);
		static FLambdaTask::ptr New(std::function<void()> run);

	public:
		FLambdaTask(std::function<void(IThreadpool&)> run);

		virtual void Run(IThreadpool& pool) noexcept override;

	protected:
		std::function<void(IThreadpool&)> lambda;
	};
}


#endif //!THREADING_THREAD_TASK_HPP
