#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/driver/autd3_device.hpp>
#include <autd3/driver/datagram/datagram.hpp>
#include <autd3/driver/datagram/force_fan.hpp>
#include <autd3/gain/focus.hpp>
#include <autd3/gain/null.hpp>
#include <autd3/gain/uniform.hpp>
#include <autd3/modulation/sine.hpp>
#include <autd3/modulation/static.hpp>
#include <coro/coro.hpp>

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
    ASSERT_THROW(autd.close(), autd3::AUTDException);
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
    ASSERT_ANY_THROW(sync_wait(autd.close_async()));
  }
}

TEST(Internal, ControllerSendTimeout) {
  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(0)));

    autd.send(autd3::gain::Null());

    autd.send(autd3::gain::Null(), std::chrono::microseconds(1));

    autd.send(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2));
  }

  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(10)));

    autd.send(autd3::gain::Null());

    autd.send(autd3::gain::Null(), std::chrono::microseconds(1));

    autd.send(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2));
  }
}

TEST(Internal, ControllerSendTimeoutAsync) {
  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(0)));

    sync_wait(autd.send_async(autd3::gain::Null()));

    sync_wait(autd.send_async(autd3::gain::Null(), std::chrono::microseconds(1)));

    sync_wait(autd.send_async(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2)));
  }

  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open(autd3::link::Audit::builder().with_timeout(std::chrono::microseconds(10)));

    sync_wait(autd.send_async(autd3::gain::Null()));

    sync_wait(autd.send_async(autd3::gain::Null(), std::chrono::microseconds(1)));

    sync_wait(autd.send_async(autd3::gain::Null(), autd3::gain::Null(), std::chrono::microseconds(2)));
  }
}

TEST(Internal, ControllerSendSingle) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Static()));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  autd.link().down();
  ASSERT_FALSE(autd.send(autd3::modulation::Static()));

  autd.link().break_down();
  ASSERT_THROW(autd.send(autd3::modulation::Static()), autd3::AUTDException);
}

TEST(Internal, ControllerSendSingleAsync) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  ASSERT_TRUE(sync_wait(autd.send_async(autd3::modulation::Static())));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
  }

  autd.link().down();
  ASSERT_FALSE(sync_wait(autd.send_async(autd3::modulation::Static())));

  autd.link().break_down();
  ASSERT_ANY_THROW(sync_wait(autd.send_async(autd3::modulation::Static())));
}

TEST(Internal, ControllerSendDouble) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Static(), autd3::gain::Uniform(0x80)));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  autd.link().down();
  ASSERT_FALSE(autd.send(autd3::modulation::Static(), autd3::gain::Uniform(1)));

  autd.link().break_down();
  ASSERT_THROW(autd.send(autd3::modulation::Static(), autd3::gain::Uniform(1)), autd3::AUTDException);
}

TEST(Internal, ControllerSendDoubleAsync) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  ASSERT_TRUE(sync_wait(autd.send_async(autd3::modulation::Static(), autd3::gain::Uniform(0x80))));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  autd.link().down();
  ASSERT_FALSE(sync_wait(autd.send_async(autd3::modulation::Static(), autd3::gain::Uniform(1))));

  autd.link().break_down();
  ASSERT_ANY_THROW(sync_wait(autd.send_async(autd3::modulation::Static(), autd3::gain::Uniform(1))));
}

TEST(Internal, ControllerGroup) {
  auto autd = create_controller();

  autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
      .set(0, autd3::modulation::Static(), autd3::gain::Null())
      .set(1, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
      .send();

  {
    const auto m = autd.link().modulation(0, autd3::native_methods::Segment::S0);
    ASSERT_EQ(2, m.size());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    const auto m = autd.link().modulation(1, autd3::native_methods::Segment::S0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
      .set(1, autd3::gain::Null())
      .set(0, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
      .send();

  {
    const auto m = autd.link().modulation(0, autd3::native_methods::Segment::S0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    auto [intensities, _] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
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
    const auto m = autd.link().modulation(0, autd3::native_methods::Segment::S0);
    ASSERT_EQ(2, m.size());
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    const auto m = autd.link().modulation(1, autd3::native_methods::Segment::S0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  sync_wait(autd.group([](auto& dev) -> std::optional<size_t> { return dev.idx(); })
                .set(1, autd3::gain::Null())
                .set(0, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80))
                .send_async());

  {
    const auto m = autd.link().modulation(0, autd3::native_methods::Segment::S0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    auto [intensities, _] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
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
      .set(0, autd3::modulation::Sine(150), autd3::gain::Uniform(0x80).with_phase(autd3::driver::Phase(0x90)))
      .send();

  ASSERT_FALSE(check[0]);
  ASSERT_TRUE(check[1]);

  {
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    const auto m = autd.link().modulation(1, autd3::native_methods::Segment::S0);
    ASSERT_EQ(80, m.size());
    auto [intensities, phases] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }
}
