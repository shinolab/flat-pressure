#include <gtest/gtest.h>

#include <autd3/modulation/square.hpp>

#include "utils.hpp"

TEST(Modulation, Square) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Square(200).with_low(32).with_high(85).with_duty(0.1)));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx());
    std::vector<uint8_t> mod_expect{85, 85, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Square(150).with_sampling_config(autd3::driver::SamplingConfiguration::from_frequency_division(10240))));

  for (auto& dev : autd.geometry()) ASSERT_EQ(10240, autd.link().modulation_frequency_division(dev.idx()));
}

TEST(Modulation, SquareWithMode) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Square(150).with_mode(autd3::native_methods::SamplingMode::SizeOptimized)));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx());
    std::vector<uint8_t> mod_expect{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
  }

  ASSERT_THROW(autd.send(autd3::modulation::Square(100.1).with_mode(autd3::native_methods::SamplingMode::ExactFrequency)), autd3::AUTDException);
  ASSERT_TRUE(autd.send(autd3::modulation::Square(100.1).with_mode(autd3::native_methods::SamplingMode::SizeOptimized)));
}

TEST(Modulation, SquareDefault) {
  const auto m = autd3::modulation::Square(150);
  ASSERT_TRUE(AUTDModulationSquareIsDefault(m.modulation_ptr()));
}
