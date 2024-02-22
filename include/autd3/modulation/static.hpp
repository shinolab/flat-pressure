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
class Static final : public driver::Modulation<Static>, public IntoCache<Static>, public IntoRadiationPressure<Static>, public IntoTransform<Static> {
 public:
  Static() : Modulation(), _intensity(driver::EmitIntensity::maximum()) {}
  explicit Static(const uint8_t intensity) : _intensity(driver::EmitIntensity(intensity)) {}
  explicit Static(const driver::EmitIntensity intensity) : _intensity(intensity) {}

  AUTD3_API [[nodiscard]] static Static with_intensity(const uint8_t intensity) { return Static(intensity); }
  AUTD3_API [[nodiscard]] static Static with_intensity(const driver::EmitIntensity intensity) { return Static(intensity); }

  AUTD3_API [[nodiscard]] driver::EmitIntensity intensity() const { return _intensity; }

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return native_methods::AUTDModulationStatic(_intensity.value(), static_cast<native_methods::LoopBehavior>(_loop_behavior));
  }

 private:
  driver::EmitIntensity _intensity;
};
}  // namespace autd3::modulation
