#include <gtest/gtest.h>

#include <autd3/def.hpp>
#include <autd3/driver/common/phase.hpp>

using autd3::driver::Phase;
using autd3::driver::rad;

TEST(DriverCommon, Phase) {
  ASSERT_EQ(0. * rad, 0. * rad);
  ASSERT_NE(0. * rad, 1. * rad);

  ASSERT_TRUE((0. * rad <=> 1. * rad) < 0);

  const auto p = Phase::from_rad(0x80 / 256.0 * 2 * autd3::driver::pi);
  ASSERT_NEAR(0x80 / 256.0 * 2 * autd3::driver::pi, p.radian(), 1e-3);
}