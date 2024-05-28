#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/controller/controller.hpp>
#include <autd3/link/audit.hpp>

TEST(Link, Audit) {
  auto autd =
      autd3::controller::ControllerBuilder().add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero())).open(autd3::link::Audit::builder());

  autd.close();
}
