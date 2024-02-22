#include <gtest/gtest.h>

#include <autd3/driver/common/emit_intensity.hpp>
#include <cmath>
#include <numbers>

TEST(Internal, EmitIntensity) {
  for (int i = 0; i <= 0xFF; i++) {
    const auto intensity = autd3::driver::EmitIntensity(static_cast<uint8_t>(i));
    ASSERT_EQ(intensity.value(), i);
  }
}

TEST(Internal, EmitIntensityWithCorrection) {
  for (int i = 0; i <= 0xFF; i++) {
    const auto intensity = autd3::driver::EmitIntensity::with_correction(static_cast<uint8_t>(i));
    ASSERT_EQ(
        intensity.value(),
        static_cast<uint8_t>(std::round(std::asin(std::pow((static_cast<double>(i) / 255.0), 1.0 / autd3::native_methods::DEFAULT_CORRECTED_ALPHA)) /
                                        std::numbers::pi * 510.0)));
  }
}
