#include <gtest/gtest.h>

#include <autd3/driver/datagram/synchronize.hpp>

#include "utils.hpp"

TEST(DriverDatagram, Synchronize) {
  auto autd = autd3::controller::ControllerBuilder()
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .open(autd3::link::Audit::builder());

  ASSERT_TRUE(autd.send(autd3::driver::Synchronize()));
}
