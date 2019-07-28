#include "threading/thread_task.hpp"
#include "threading/thread_pull.hpp"


namespace threading
{
	//********************************************************
	//						  Tasks
	//********************************************************

	FTasks::ptr FTasks::New()
	{
		return std::make_unique<FTasks>();
	}

	void FTasks::Run(IThreadpool& pool)
	{
		for (auto& task : tasks)
		{
			if (hook)
			{
				task->Next(hook);
			}
			pool.AddTask(std::move(task));
		}
		if (hook)
		{
			pool.AddTask(std::move(hook));
		}
	}
	
	void FTasks::SetHook(FTask::ptr&& task)
	{
		hook = std::move(task);
	}
	
	void FTasks::AddTask(FTask::ptr&& task)
	{
		tasks.emplace_back(std::move(task));
	}

	//********************************************************
	//						  Task
	//********************************************************

	int FTask::NPrev() const
	{
		return prev;
	}

	int FTask::NNext() const
	{
		return int(nexts.size());
	}

	void FTask::SetOnDone(event event)
	{
		onDone = event;
	}

	void FTask::OnPrevDone()
	{
		--prev;
	}

	void FTask::OnFinished()
	{
		for (auto* next : nexts)
		{
			next->OnPrevDone();
		}
		
		if (onDone)
		{
			onDone();
		}
	}

	//********************************************************
	//						  LambdaTask
	//********************************************************

	FLambdaTask::ptr FLambdaTask::New(std::function<void(IThreadpool&)> run)
	{
		return std::make_unique<FLambdaTask>(run);
	}

	FLambdaTask::ptr FLambdaTask::New(std::function<void()> run)
	{
		return std::make_unique<FLambdaTask>([run](IThreadpool&) { run(); });
	}

	FLambdaTask::FLambdaTask(std::function<void(IThreadpool&)> run)
		: lambda(run)
	{}

	void FLambdaTask::Run(IThreadpool& pool)
	{
		lambda(pool);
	}
}
