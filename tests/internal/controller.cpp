// File: controller.cpp
// Project: internal
// Created Date: 26/09/2023
// Author: Shun Suzuki
// -----
// Last Modified: 05/01/2024
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2023 Shun Suzuki. All rights reserved.
//

#include <gtest/gtest.h>

#include <autd3/datagram/force_fan.hpp>
#include <autd3/gain/focus.hpp>
#include <autd3/gain/null.hpp>
#include <autd3/gain/uniform.hpp>
#include <autd3/internal/datagram.hpp>
#include <autd3/modulation/sine.hpp>
#include <autd3/modulation/static.hpp>

#include "utils.hpp"

TEST(Internal, ControllerClose) {
  {
    const auto autd = create_controller();
    ASSERT_TRUE(autd.link().is_open());

    autd.close();
    ASSERT_FALSE(autd.link().is_open());
  }

  {
    const auto autd = create_controller();
    autd.link().break_down();
    ASSERT_THROW(autd.close(), autd3::internal::AUTDException);
  }
}

TEST(Internal, ControllerCloseAsync) {
  {
    const auto autd = create_controller();
    ASSERT_TRUE(autd.link().is_open());

    sync_wait(autd.close_async());
    ASSERT_FALSE(autd.link().is_open());
  }

  {
    const auto autd = create_controller();
    autd.link().break_down();
    ASSERT_THROW(sync_wait(autd.close_async()), autd3::internal::AUTDException);
  }
}

TEST(Internal, ControllerSendTimeout) {
  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(0)));

    autd.send(autd3::gain::Null());
    ASSERT_EQ(autd.link().last_timeout_ns(), 0);

    autd.send(autd3::gain::Null(), std::chrono::microseconds(1));
    ASSERT_EQ(autd.link().last_timeout_ns(), 1000);

    autd.send(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2));
    ASSERT_EQ(autd.link().last_timeout_ns(), 2000);
  }

  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(10)));

    autd.send(autd3::gain::Null());
    ASSERT_EQ(autd.link().last_timeout_ns(), 10000);

    autd.send(autd3::gain::Null(), std::chrono::microseconds(1));
    ASSERT_EQ(autd.link().last_timeout_ns(), 1000);

    autd.send(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2));
    ASSERT_EQ(autd.link().last_timeout_ns(), 2000);
  }
}

TEST(Internal, ControllerSendTimeoutAsync) {
  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(0)));

    sync_wait(autd.send_async(autd3::gain::Null()));
    ASSERT_EQ(autd.link().last_timeout_ns(), 0);

    sync_wait(autd.send_async(autd3::gain::Null(), std::chrono::microseconds(1)));
    ASSERT_EQ(autd.link().last_timeout_ns(), 1000);

    sync_wait(autd.send_async(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2)));
    ASSERT_EQ(autd.link().last_timeout_ns(), 2000);
  }

  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(10)));

    sync_wait(autd.send_async(autd3::gain::Null()));
    ASSERT_EQ(autd.link().last_timeout_ns(), 10000);

    sync_wait(autd.send_async(autd3::gain::Null(), std::chrono::microseconds(1)));
    ASSERT_EQ(autd.link().last_timeout_ns(), 1000);

    sync_wait(autd.send_async(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2)));
    ASSERT_EQ(autd.link().last_timeout_ns(), 2000);
  }
}

TEST(Internal, ControllerSendSingle) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Static()));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  autd.link().down();
  ASSERT_FALSE(autd.send(autd3::modulation::Static()));

  autd.link().break_down();
  ASSERT_THROW(autd.send(autd3::modulation::Static()), autd3::internal::AUTDException);
}

TEST(Internal, ControllerSendSingleAsync) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  ASSERT_TRUE(sync_wait(autd.send_async(autd3::modulation::Static())));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  autd.link().down();
  ASSERT_FALSE(sync_wait(autd.send_async(autd3::modulation::Static())));

  autd.link().break_down();
  ASSERT_THROW(sync_wait(autd.send_async(autd3::modulation::Static())), autd3::internal::AUTDException);
}

TEST(Internal, ControllerSendDouble) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().intensities_and_phases(dev.idx(), 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Static(), autd3::gain::Uniform(0x80)));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().intensities_and_phases(dev.idx(), 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  autd.link().down();
  ASSERT_FALSE(autd.send(autd3::modulation::Static(), autd3::gain::Uniform(1)));

  autd.link().break_down();
  ASSERT_THROW(autd.send(autd3::modulation::Static(), autd3::gain::Uniform(1)), autd3::internal::AUTDException);
}

TEST(Internal, ControllerSendDoubleAsync) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().intensities_and_phases(dev.idx(), 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  ASSERT_TRUE(sync_wait(autd.send_async(autd3::modulation::Static(), autd3::gain::Uniform(0x80))));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().intensities_and_phases(dev.idx(), 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  autd.link().down();
  ASSERT_FALSE(sync_wait(autd.send_async(autd3::modulation::Static(), autd3::gain::Uniform(1))));

  autd.link().break_down();
  ASSERT_THROW(sync_wait(autd.send_async(autd3::modulation::Static(), autd3::gain::Uniform(1))), autd3::internal::AUTDException);
}

TEST(Internal, ControllerGroup) {
  auto autd = create_controller();

  autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
      .set(0, autd3::modulation::Static(), autd3::gain::Null())
      .set(1, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
      .send();

  {
    const auto m = autd.link().modulation(0);
    ASSERT_EQ(2, m.size());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().intensities_and_phases(0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    const auto m = autd.link().modulation(1);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().intensities_and_phases(1, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
      .set(1, autd3::gain::Null())
      .set(0, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
      .send();

  {
    const auto m = autd.link().modulation(0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().intensities_and_phases(0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    auto [intensities, _] = autd.link().intensities_and_phases(1, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
  }
}

TEST(Internal, ControllerGroupAsync) {
  auto autd = create_controller();

  sync_wait(autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
                .set(0, autd3::modulation::Static(), autd3::gain::Null())
                .set(1, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
                .send_async());

  {
    const auto m = autd.link().modulation(0);
    ASSERT_EQ(2, m.size());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().intensities_and_phases(0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    const auto m = autd.link().modulation(1);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().intensities_and_phases(1, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  sync_wait(autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
                .set(1, autd3::gain::Null())
                .set(0, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
                .send_async());

  {
    const auto m = autd.link().modulation(0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().intensities_and_phases(0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    auto [intensities, _] = autd.link().intensities_and_phases(1, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
  }
}

TEST(Internal, ControllerGroupCheckOnlyForEnabled) {
  auto autd = create_controller();
  autd.geometry()[0].set_enable(false);

  std::vector check(autd.geometry().num_devices(), false);
  autd.group([&check](auto& dev) -> std::optional<int> {
        check[dev.idx()] = true;
        return 0;
      })
      .set(0, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80).with_phase(autd3::internal::Phase(0x90)))
      .send();

  ASSERT_FALSE(check[0]);
  ASSERT_TRUE(check[1]);

  {
    auto [intensities, phases] = autd.link().intensities_and_phases(0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    const auto m = autd.link().modulation(1);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().intensities_and_phases(1, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }
}

TEST(Internal_Geometry, DeviceForceFan) {
  auto autd = create_controller();
  for (auto& dev : autd.geometry()) ASSERT_FALSE(autd.link().is_force_fan(dev.idx()));

  autd.send(autd3::datagram::ConfigureForceFan([](const auto& dev) { return dev.idx() == 0; }));
  ASSERT_TRUE(autd.link().is_force_fan(0));
  ASSERT_FALSE(autd.link().is_force_fan(1));

  autd.send(autd3::datagram::ConfigureForceFan([](const auto& dev) { return dev.idx() == 1; }));
  ASSERT_FALSE(autd.link().is_force_fan(0));
  ASSERT_TRUE(autd.link().is_force_fan(1));
}