#pragma once

#include <autd3/controller/builder.hpp>
#include <autd3/controller/controller.hpp>
#include <autd3/driver/autd3_device.hpp>
#include <autd3/link/audit.hpp>

static inline autd3::controller::Controller<autd3::link::Audit> create_controller() {
  return coro::sync_wait(autd3::controller::ControllerBuilder()
                             .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                             .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                             .open_with_async(autd3::link::Audit::builder()));
}

#define ASSERT_NEAR_VECTOR3(val1, val2, abs_error) \
  do {                                             \
    ASSERT_NEAR(val1.x(), val2.x(), abs_error);    \
    ASSERT_NEAR(val1.y(), val2.y(), abs_error);    \
    ASSERT_NEAR(val1.z(), val2.z(), abs_error);    \
  } while (false)
