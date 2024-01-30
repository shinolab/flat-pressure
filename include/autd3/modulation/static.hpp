#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/modulation/transform.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

/**
 * @brief Without modulation
 */
class Static final : public driver::Modulation, public IntoCache<Static>, public IntoRadiationPressure<Static>, public IntoTransform<Static> {
 public:
  Static() : _intensity(driver::EmitIntensity::maximum()) {}
  explicit Static(const uint8_t intensity) : _intensity(driver::EmitIntensity(intensity)) {}
  explicit Static(const driver::EmitIntensity intensity) : _intensity(intensity) {}

  static Static with_intensity(const uint8_t intensity) { return Static(intensity); }
  static Static with_intensity(const driver::EmitIntensity intensity) { return Static(intensity); }

  [[nodiscard]] driver::EmitIntensity intensity() const { return _intensity; }

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override { return native_methods::AUTDModulationStatic(_intensity.value()); }

 private:
  driver::EmitIntensity _intensity;
};
}  // namespace autd3::modulation
