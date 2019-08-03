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

	void FTasks::Run(IThreadpool& pool) noexcept
	{
		FTask::Run(pool);
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

	void FTask::Run(IThreadpool& pool) noexcept
	{
		if (onBegin)
		{
			onBegin();
		}
	}

	int FTask::NPrev() const noexcept
	{
		return prev;
	}

	int FTask::NNext() const noexcept
	{
		return int(nexts.size());
	}

	void FTask::SetOnBegin(event event) noexcept
	{
		onBegin = event;
	}
	
	void FTask::SetOnDone(event event) noexcept
	{
		onDone = event;
	}

	void FTask::OnPrevDone() noexcept
	{
		--prev;
	}

	void FTask::OnFinished() noexcept
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

	void FLambdaTask::Run(IThreadpool& pool) noexcept
	{
		FTask::Run(pool);
		lambda(pool);
	}
}
