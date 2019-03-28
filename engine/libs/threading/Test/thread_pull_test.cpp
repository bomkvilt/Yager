#include "gtest/gtest.h"
#include "threading/thread_impl.hpp"


struct pool_tests : testing::Test
{
	void SetUp() override
	{
		conf.threads = 2;
	}
	FThreadingConfig conf;
};


TEST_F(pool_tests, simple)
{	// here we run more tasks than threads we have
	std::condition_variable cv;

	std::string outs = "";
	auto a = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'a'; });
	auto b = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'b'; });
	auto c = threading::FLambdaTask::New([&](threading::IThreadpool&) { outs += 'c'; });
	auto e = threading::FLambdaTask::New([&](threading::IThreadpool&) { cv.notify_one(); });

	auto pool = threading::Threadpool();
	pool.Construct(conf);
	pool.AddTask(std::move(a));
	pool.AddTask(std::move(b));
	pool.AddTask(std::move(c));
	pool.AddTask(std::move(e));
	
	auto mu = std::mutex();
	auto lk = std::unique_lock(mu);
	cv.wait(lk);

	EXPECT_EQ(outs, "abc");
}
