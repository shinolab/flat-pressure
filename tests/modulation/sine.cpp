#include <gtest/gtest.h>

#include <autd3/modulation/sine.hpp>

#include "utils.hpp"

TEST(Modulation, Sine) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Sine(150)
                            .with_intensity(autd3::driver::EmitIntensity::maximum() / 2)
                            .with_offset(autd3::driver::EmitIntensity::maximum() / 4)
                            .with_phase(autd3::driver::Phase::from_rad(autd3::driver::pi / 2))));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx());
    std::vector<uint8_t> mod_expect{126, 124, 119, 111, 100, 87, 73, 58, 44, 30, 18, 9, 3, 0, 1, 5, 12, 22, 34, 48, 63, 78, 92,  104, 114, 121, 125,
                                    126, 123, 117, 108, 96,  82, 68, 53, 39, 26, 15, 7, 2, 0, 2, 7, 15, 26, 39, 53, 68, 82, 96,  108, 117, 123, 126,
                                    125, 121, 114, 104, 92,  78, 63, 48, 34, 22, 12, 5, 1, 0, 3, 9, 18, 30, 44, 58, 73, 87, 100, 111, 119, 124};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Sine(150).with_sampling_config(autd3::driver::SamplingConfiguration::from_frequency_division(10240))));
  for (auto& dev : autd.geometry()) ASSERT_EQ(10240, autd.link().modulation_frequency_division(dev.idx()));
}

TEST(Modulation, SineWithMode) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Sine(150).with_mode(autd3::native_methods::SamplingMode::SizeOptimized)));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx());
    std::vector<uint8_t> mod_expect{127, 156, 184, 209, 229, 244, 252, 254, 249, 237, 219, 197, 170, 142,
                                    112, 84,  57,  35,  17,  5,   0,   2,   10,  25,  45,  70,  98};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
  }

  ASSERT_THROW(autd.send(autd3::modulation::Sine(100.1).with_mode(autd3::native_methods::SamplingMode::ExactFrequency)), autd3::AUTDException);
  ASSERT_TRUE(autd.send(autd3::modulation::Sine(100.1).with_mode(autd3::native_methods::SamplingMode::SizeOptimized)));
}

TEST(Modulation, SineDefault) {
  const auto m = autd3::modulation::Sine(150);
  ASSERT_TRUE(AUTDModulationSineIsDefault(m.modulation_ptr()));
}
