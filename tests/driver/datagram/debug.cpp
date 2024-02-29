#include <gtest/gtest.h>

#include <autd3/driver/datagram/debug.hpp>

#include "utils.hpp"

TEST(DriverDatagram, ConfigureDebugOutputIdx) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(0xFF, autd.link().debug_output_idx(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureDebugOutputIdx([](const autd3::driver::geometry::Device& dev) { return &dev[0]; })));
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(0, autd.link().debug_output_idx(dev.idx()));
  }

  ASSERT_TRUE(autd.send(
      autd3::driver::ConfigureDebugOutputIdx([](const autd3::driver::geometry::Device& dev) { return dev.idx() == 0 ? &dev[10] : nullptr; })));
  ASSERT_EQ(10, autd.link().debug_output_idx(0));
  ASSERT_EQ(0xFF, autd.link().debug_output_idx(1));
}
