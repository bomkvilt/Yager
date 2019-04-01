#include "gtest/gtest.h"
#include "threading/thread_impl.hpp"


struct pool_tests : testing::Test
{
	void SetUp() override
	{
		for (auto i : { 1, 2, 8 })
		{
			FThreadingConfig conf;
			conf.threads = i;

			confs[i] = conf;
			pools[i].Construct(conf);
		}
	}

	std::map<int, FThreadingConfig>		 confs;
	std::map<int, threading::Threadpool> pools;

	int nstress = 1000;

	void stress(threading::Threadpool& pool)
	{
		std::condition_variable cv;

		using namespace threading;
		
		for (auto i = 0; i < nstress; ++i)
		{
			pool.AddTask(FLambdaTask::New([&]() {}));
		}
		pool.AddTask(FLambdaTask::New([&]() { cv.notify_one(); }));

		auto mu = std::mutex();
		auto lk = std::unique_lock(mu);
		cv.wait(lk);
	}
};


TEST_F(pool_tests, simple)
{
	std::condition_variable cv;

	using namespace threading;

	std::atomic_int outs = 0;
	auto a = FLambdaTask::New([&]() { ++outs; });
	auto b = FLambdaTask::New([&]() { ++outs; });
	auto c = FLambdaTask::New([&]() { ++outs; });
	auto e = FLambdaTask::New([&]() { cv.notify_one(); });

	a->Next(e);
	b->Next(e);
	c->Next(e);

	auto& pool = pools[2];
	pool.AddTask(std::move(a));
	pool.AddTask(std::move(b));
	pool.AddTask(std::move(c));
	pool.AddTask(std::move(e));
	
	auto mu = std::mutex();
	auto lk = std::unique_lock(mu);
	cv.wait(lk);

	EXPECT_EQ(outs, 3);
}

TEST_F(pool_tests, stress_termination)
{	// this object have several workers insede
	// and we need to ensure that the object terminates as well
	using namespace threading;

	const auto times = 100;
	for (auto i = 0; i < times; ++i)
	{
		auto pool = Threadpool();
		pool.Construct(confs[8]);
	}
	// assert = test haven't crash
}

TEST_F(pool_tests, stress_1)
{
	stress(pools[1]);
}

TEST_F(pool_tests, stress_2)
{
	stress(pools[2]);
}

TEST_F(pool_tests, stress_8)
{
	stress(pools[8]);
}
