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
    auto mod = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    std::vector<uint8_t> mod_expect{127, 125, 120, 111, 100, 87, 73, 58, 43, 30, 18, 9, 3, 0, 0, 4, 12, 22, 34, 48, 63, 78, 92,  104, 114, 122, 126,
                                    126, 123, 117, 108, 96,  83, 68, 53, 39, 26, 15, 6, 1, 0, 1, 6, 15, 26, 39, 53, 68, 83, 96,  108, 117, 123, 126,
                                    126, 122, 114, 104, 92,  78, 63, 48, 34, 22, 12, 4, 0, 0, 3, 9, 18, 30, 43, 58, 73, 87, 100, 111, 120, 125};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Sine(150).with_sampling_config(autd3::driver::SamplingConfiguration::from_frequency_division(10240))));
  for (auto& dev : autd.geometry()) ASSERT_EQ(10240, autd.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
}

TEST(Modulation, SineWithMode) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Sine(150).with_mode(autd3::native_methods::SamplingMode::SizeOptimized)));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    std::vector<uint8_t> mod_expect{127, 156, 184, 209, 229, 244, 253, 254, 249, 237, 220, 197, 171, 142,
                                    112, 83,  57,  34,  17,  5,   0,   1,   10,  25,  45,  70,  98};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
  }

  ASSERT_THROW(autd.send(autd3::modulation::Sine(100.1).with_mode(autd3::native_methods::SamplingMode::ExactFrequency)), autd3::AUTDException);
  ASSERT_TRUE(autd.send(autd3::modulation::Sine(100.1).with_mode(autd3::native_methods::SamplingMode::SizeOptimized)));
}

TEST(Modulation, SineDefault) {
  const auto m = autd3::modulation::Sine(150);
  ASSERT_TRUE(AUTDModulationSineIsDefault(m.modulation_ptr()));
}
