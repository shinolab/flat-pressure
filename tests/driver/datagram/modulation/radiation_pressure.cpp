#include <gtest/gtest.h>

#include <autd3/modulation/sine.hpp>

#include "utils.hpp"

TEST(DriverDatagramModulation, RadiationPressure) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Sine(150).with_radiation_pressure()));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    std::vector<uint8_t> mod_expect{
        180, 200, 217, 231, 242, 250, 254, 254, 251, 245, 235, 222, 206, 187, 165, 141, 115, 87, 58, 28, 0,  28, 58, 87,  115, 141, 165,
        187, 206, 222, 235, 245, 251, 254, 254, 250, 242, 231, 217, 200, 180, 157, 133, 106, 78, 48, 16, 0,  39, 68, 97,  124, 150, 173,
        194, 212, 227, 239, 248, 253, 255, 253, 248, 239, 227, 212, 194, 173, 150, 124, 97,  68, 39, 0,  16, 48, 78, 106, 133, 157,
    };
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }
}
