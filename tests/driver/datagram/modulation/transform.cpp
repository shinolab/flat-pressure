#include <gtest/gtest.h>

#include <autd3/modulation/sine.hpp>

#include "utils.hpp"

TEST(DriverDatagramModulation, Transform) {
  auto autd1 = create_controller();
  auto autd2 = create_controller();

  ASSERT_TRUE(autd1.send(autd3::modulation::Sine(150)));
  ASSERT_TRUE(autd2.send(autd3::modulation::Sine(150).with_transform(
      [](size_t, const autd3::driver::EmitIntensity v) { return autd3::driver::EmitIntensity(v.value() / 2); })));

  for (auto& dev : autd1.geometry()) {
    auto mod_expect = autd1.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    std::ranges::transform(mod_expect, mod_expect.begin(), [](const uint8_t x) { return x / 2; });
    auto mod = autd2.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd2.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }
}
