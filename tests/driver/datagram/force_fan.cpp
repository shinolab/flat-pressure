#include <gtest/gtest.h>

#include <autd3/driver/datagram/force_fan.hpp>

#include "utils.hpp"

TEST(DriverDatagram, ForceFan) {
  auto autd = create_controller();
  for (auto& dev : autd.geometry()) ASSERT_FALSE(autd.link().is_force_fan(dev.idx()));

  autd.send(autd3::driver::ConfigureForceFan([](const auto& dev) { return dev.idx() == 0; }));
  ASSERT_TRUE(autd.link().is_force_fan(0));
  ASSERT_FALSE(autd.link().is_force_fan(1));

  autd.send(autd3::driver::ConfigureForceFan([](const auto& dev) { return dev.idx() == 1; }));
  ASSERT_FALSE(autd.link().is_force_fan(0));
  ASSERT_TRUE(autd.link().is_force_fan(1));
}
