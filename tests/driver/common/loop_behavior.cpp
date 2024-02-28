#include <gtest/gtest.h>

#include <autd3/driver/common/loop_behavior.hpp>

using autd3::driver::LoopBehavior;

TEST(DriverCommon, LoopBehavior) {
  ASSERT_THROW((void)LoopBehavior::finite(0), autd3::AUTDException);

  ASSERT_EQ(LoopBehavior::finite(1), LoopBehavior::once());
  ASSERT_NE(LoopBehavior::finite(1), LoopBehavior::infinite());

  ASSERT_TRUE((LoopBehavior::finite(1) <=> LoopBehavior::finite(2)) < 0);
  ASSERT_TRUE((LoopBehavior::finite(2) <=> LoopBehavior::infinite()) < 0);
}