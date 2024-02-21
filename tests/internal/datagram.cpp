#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/driver/datagram/clear.hpp>
#include <autd3/driver/datagram/datagram.hpp>
#include <autd3/driver/datagram/debug.hpp>
#include <autd3/driver/datagram/silencer.hpp>
#include <autd3/driver/datagram/synchronize.hpp>
#include <autd3/gain/uniform.hpp>

#include "utils.hpp"

TEST(Internal, Silencer) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(10, autd.link().silencer_completion_steps_intensity(dev.idx()));
    ASSERT_EQ(40, autd.link().silencer_completion_steps_phase(dev.idx()));
    ASSERT_TRUE(autd.link().silencer_fixed_completion_steps_mode(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureSilencer::fixed_completion_steps(1, 2)));
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(1, autd.link().silencer_completion_steps_intensity(dev.idx()));
    ASSERT_EQ(2, autd.link().silencer_completion_steps_phase(dev.idx()));
    ASSERT_TRUE(autd.link().silencer_fixed_completion_steps_mode(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureSilencer::disable()));
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(1, autd.link().silencer_completion_steps_intensity(dev.idx()));
    ASSERT_EQ(1, autd.link().silencer_completion_steps_phase(dev.idx()));
    ASSERT_TRUE(autd.link().silencer_fixed_completion_steps_mode(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureSilencer::default_()));
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(10, autd.link().silencer_completion_steps_intensity(dev.idx()));
    ASSERT_EQ(40, autd.link().silencer_completion_steps_phase(dev.idx()));
    ASSERT_TRUE(autd.link().silencer_fixed_completion_steps_mode(dev.idx()));
  }
}

TEST(Internal, ConfigureDebugOutputIdx) {
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

TEST(Internal, Clear) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::Uniform(0x80).with_phase(autd3::driver::Phase(0x90))));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }

  ASSERT_TRUE(autd.send(autd3::driver::Clear()));
  for (auto& dev : autd.geometry()) {
    auto m = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::all_of(m, [](auto d) { return d == 0xFF; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
}

TEST(Internal, Synchronize) {
  auto autd = autd3::controller::ControllerBuilder()
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .open(autd3::link::Audit::builder());

  ASSERT_TRUE(autd.send(autd3::driver::Synchronize()));
}
