#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain::holo {

class IEmissionConstraint {
 public:
  virtual native_methods::EmissionConstraintPtr ptr() const = 0;

  auto operator<=>(const IEmissionConstraint& other) const { return AUTDGainHoloConstraintEq(ptr(), other.ptr()); }
};

/**
 * @brief Amplitude constraint
 */
class EmissionConstraint final {
 public:
  /**
   * @brief Do nothing (this is equivalent to `Clamp(0, 1)`)
   */
  static EmissionConstraint dont_care() { return EmissionConstraint{native_methods::AUTDGainHoloConstraintDotCare()}; }

  /**
   * @brief Normalize the value by dividing the maximum value
   */
  static EmissionConstraint normalize() { return EmissionConstraint{native_methods::AUTDGainHoloConstraintNormalize()}; }

  /**
   * @brief Set all amplitudes to the specified value
   * @param value amplitude
   */
  static EmissionConstraint uniform(const driver::EmitIntensity value) {
    return EmissionConstraint{native_methods::AUTDGainHoloConstraintUniform(value.value())};
  }

  /**
   * @brief Set all amplitudes to the specified value
   * @param value amplitude
   */
  static EmissionConstraint uniform(const uint8_t value) { return uniform(driver::EmitIntensity(value)); }

  /**
   * @brief Clamp all amplitudes to the specified range
   *
   * @param min_v minimum amplitude
   * @param max_v maximum amplitude
   */
  static EmissionConstraint clamp(const driver::EmitIntensity min_v, const driver::EmitIntensity max_v) {
    return EmissionConstraint{native_methods::AUTDGainHoloConstraintClamp(min_v.value(), max_v.value())};
  }

  /**
   * @brief Clamp all amplitudes to the specified range
   *
   * @param min_v minimum amplitude
   * @param max_v maximum amplitude
   */
  static EmissionConstraint clamp(const uint8_t min_v, const uint8_t max_v) {
    return clamp(driver::EmitIntensity(min_v), driver::EmitIntensity(max_v));
  }

  [[nodiscard]] native_methods::EmissionConstraintPtr ptr() const { return _ptr; }

 private:
  explicit EmissionConstraint(const native_methods::EmissionConstraintPtr ptr) : _ptr(ptr) {}

  native_methods::EmissionConstraintPtr _ptr;
};

}  // namespace autd3::gain::holo
