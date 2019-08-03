#include "gtest/gtest.h"
#include "threading/thread_impl.hpp"


struct pool_tests : testing::Test
{
	void SetUp() override
	{
		for (auto i : { 1, 2, 7 })
		{
			FThreadingConfig conf;
			conf.threads = i;

			confs[i] = conf;
			pools[i].Construct(conf);
		}
	}

	std::map<int, FThreadingConfig>		 confs;
	std::map<int, threading::Threadpool> pools;

	int nstress = int(10e2);

	void stress(threading::Threadpool& pool, int threads)
	{
		auto done = std::atomic_int(0);
		auto perThread  = nstress   / threads;
		auto totalCount = perThread * threads;

		using namespace threading;

		for (int n = 0; n < threads; ++n)
		{	// destribute taks per worker
			auto buildLock = std::condition_variable();
			pool.AddTask(FLambdaTask::New([&done, &buildLock, &pool, perThread]()
			{	// spawn tasks
				buildLock.notify_all();
				for (auto i = 0; i < perThread; ++i)
				{
					pool.AddTask(FLambdaTask::New([&done]()
					{
						uint64_t a = 10;
						for (auto i = 0; i < 300; ++i)
						{
							a *= a;
						}
						++done;
					}));
				}
			}));
			auto mu = std::mutex();
			auto lk = std::unique_lock(mu);
			buildLock.wait(lk);
		}
		
		while (done < totalCount)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1us);
		}
	}
};


TEST_F(pool_tests, simple)
{
	std::condition_variable cv;

	using namespace threading;

	std::atomic_int outs = 0;
	auto k = false;
	auto a = FLambdaTask::New([&]() { ++outs; });
	auto b = FLambdaTask::New([&]() { ++outs; });
	auto c = FLambdaTask::New([&]() { ++outs; });
	auto e = FLambdaTask::New([&]() { cv.notify_one(); });
	c->SetOnDone([&]() { k = true; });

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
	EXPECT_TRUE(k);
}

TEST_F(pool_tests, stress_termination)
{	// this object have several workers insede
	// and we need to ensure that the object terminates as well
	using namespace threading;

	const auto times = 30;
	for (auto i = 0; i < times; ++i)
	{
		auto pool = Threadpool();
		pool.Construct(confs[8]);
	}
	// assert = test didn't crash
}

TEST_F(pool_tests, stress_1)
{
	stress(pools[1], 1);
	// assert = test didn't crash
}

TEST_F(pool_tests, stress_2)
{
	stress(pools[2], 2);
	// assert = test didn't crash
}

TEST_F(pool_tests, stress_7)
{
	stress(pools[7], 7);
	// assert = test didn't crash
}
