#pragma once

#include "autd3/native_methods.hpp"

namespace autd3::driver {

class EmitIntensity final {
 public:
  AUTD3_API static EmitIntensity maximum() { return EmitIntensity{255}; }
  AUTD3_API static EmitIntensity minimum() { return EmitIntensity{0}; }

  AUTD3_API explicit EmitIntensity(const uint8_t value) : _value(value) {}

  AUTD3_API [[nodiscard]] static EmitIntensity with_correction_alpha(const uint8_t value, const double alpha) {
    return EmitIntensity(native_methods::AUTDEmitIntensityWithCorrectionAlpha(value, alpha));
  }

  AUTD3_API [[nodiscard]] static EmitIntensity with_correction(const uint8_t value) {
    return with_correction_alpha(value, native_methods::DEFAULT_CORRECTED_ALPHA);
  }

  AUTD3_API [[nodiscard]] uint8_t value() const noexcept { return _value; }

  AUTD3_API friend EmitIntensity operator/(EmitIntensity&& lhs, const int& rhs) { return EmitIntensity(static_cast<uint8_t>(lhs._value / rhs)); }
  AUTD3_API auto operator<=>(const EmitIntensity&) const = default;

 private:
  uint8_t _value;
};

}  // namespace autd3::driver
