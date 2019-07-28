#include "gtest/gtest.h"
#include "common/hit_timer.hpp"


struct hit_timer_tests : public testing::Test
{};


TEST_F(hit_timer_tests, simple)
{
	auto hits = HitTimer(2);
	ASSERT_FALSE(hits.Hit());
	ASSERT_TRUE (hits.Hit());
	ASSERT_FALSE(hits.Hit());
}

TEST_F(hit_timer_tests, custom_hits_step)
{
	auto hits = HitTimer(4);
	ASSERT_FALSE(hits.Hit());
	ASSERT_FALSE(hits.Hit());
	hits.SetNorm(2);
	ASSERT_TRUE (hits.Hit());
	ASSERT_FALSE(hits.Hit());
}
