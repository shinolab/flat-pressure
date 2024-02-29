#include <gtest/gtest.h>

#include <autd3/driver/datagram/silencer.hpp>

#include "utils.hpp"

TEST(DriverDatagram, Silencer) {
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

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureSilencer::fixed_update_rate(10, 20)));
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(10, autd.link().silencer_update_rate_intensity(dev.idx()));
    ASSERT_EQ(20, autd.link().silencer_update_rate_phase(dev.idx()));
    ASSERT_FALSE(autd.link().silencer_fixed_completion_steps_mode(dev.idx()));
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

  ASSERT_TRUE(
      autd3::native_methods::AUTDDatagramSilencerFixedCompletionStepsIsDefault(autd3::driver::ConfigureSilencer::default_().ptr(autd.geometry())));
}