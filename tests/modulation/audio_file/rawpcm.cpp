#include <gtest/gtest.h>

#include "autd3/modulation/audio_file.hpp"
#include "utils.hpp"

TEST(Modulation, RawPCM) {
  auto autd = create_controller();

  const std::filesystem::path path = std::filesystem::path(AUTD3_RESOURCE_PATH).append("sin150.dat");
  ASSERT_TRUE(autd.send(autd3::modulation::audio_file::RawPCM(path, 4000)));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx());
    std::vector<uint8_t> mod_expect{157, 185, 210, 231, 245, 253, 255, 249, 236, 218, 194, 167, 138, 108, 79,  53,  31,  14,  4,   0,
                                    4,   14,  31,  53,  79,  108, 138, 167, 194, 218, 236, 249, 255, 253, 245, 231, 210, 185, 157, 128,
                                    98,  70,  45,  24,  10,  2,   0,   6,   19,  37,  61,  88,  117, 147, 176, 202, 224, 241, 251, 255,
                                    251, 241, 224, 202, 176, 147, 117, 88,  61,  37,  19,  6,   0,   2,   10,  24,  45,  70,  98,  128};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx()));
  }

  ASSERT_TRUE(autd.send(
      autd3::modulation::audio_file::RawPCM(path, 4000).with_sampling_config(autd3::driver::SamplingConfiguration::from_frequency_division(10240))));
  for (auto& dev : autd.geometry()) ASSERT_EQ(10240, autd.link().modulation_frequency_division(dev.idx()));
}

TEST(Modulation, RawPCMDefault) {
  const std::filesystem::path path = std::filesystem::path(AUTD3_RESOURCE_PATH).append("sin150.dat");
  const auto m = autd3::modulation::audio_file::RawPCM(path, 4000);
  ASSERT_TRUE(autd3::native_methods::AUTDSamplingConfigEq(static_cast<autd3::native_methods::SamplingConfiguration>(m.sampling_config()),
                                                          autd3::native_methods::AUTDModulationRawPCMDefaultSamplingConfig()));
}
