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

	int FTask::Prevs() const
	{
		return prev;
	}

	int FTask::Nexts() const
	{
		return int(nexts.size());
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
	}

	//********************************************************
	//						  LambdaTask
	//********************************************************

	FLambdaTask::ptr FLambdaTask::New(clb run)
	{
		return std::make_unique<FLambdaTask>(run);
	}

	FLambdaTask::FLambdaTask(clb run)
		: lambda(run)
	{}

	void FLambdaTask::Run(IThreadpool& pool)
	{
		lambda(pool);
	}
}
