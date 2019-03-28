#include "gtest/gtest.h"
#include "threading/thread_pull.hpp"


struct task_tests : public testing::Test
{
	struct DummyPool : public threading::IThreadpool
	{
		virtual void Construct(FThreadingConfig newConfig) {}
		virtual void AddTask(threading::FTask::ptr&& task) {}
	};

	DummyPool dummy_pool;
};


TEST_F(task_tests, simple)
{
	int val = 10;
	threading::FLambdaTask::New([&](threading::IThreadpool&) { val = 30; })->Run(dummy_pool);
	EXPECT_EQ(val, 30);
}

TEST_F(task_tests, chain)
{
	auto a = threading::FLambdaTask::New([&](threading::IThreadpool&) {});
	auto b = threading::FLambdaTask::New([&](threading::IThreadpool&) {});
	auto c = threading::FLambdaTask::New([&](threading::IThreadpool&) {});
	
	a->Next(c);
	b->Next(c);

	EXPECT_EQ(a->Prevs(), 0);
	EXPECT_EQ(b->Prevs(), 0);
	EXPECT_EQ(c->Prevs(), 2);

	EXPECT_EQ(a->Nexts(), 1);
	EXPECT_EQ(b->Nexts(), 1);
	EXPECT_EQ(c->Nexts(), 0);
}

TEST_F(task_tests, tasks)
{
	struct TestPool : public threading::IThreadpool
	{
		virtual void Construct(FThreadingConfig newConfig) {}
		virtual void AddTask(threading::FTask::ptr&& task) 
		{
			task->Run(*this);
		}
	};
	auto outs = std::string();
	auto pool = TestPool();

	auto a = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'a'; });
	auto b = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'b'; });
	auto c = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'c'; });
	auto h = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'h'; });

	auto s = threading::FTasks::New();
	s->AddTask(std::move(a));
	s->AddTask(std::move(b));
	s->AddTask(std::move(c));
	s->SetHook(std::move(h));
	pool.AddTask(std::move(s));

	EXPECT_EQ(outs, "abch");
}
