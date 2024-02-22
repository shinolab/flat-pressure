#pragma once

#include <algorithm>

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/cache.hpp"
#include "autd3/gain/transform.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain {

/**
 * @brief Gain to set intensity and phase uniformly
 */
class Uniform final : public driver::Gain<Uniform>, public IntoCache<Uniform>, public IntoTransform<Uniform> {
 public:
  explicit Uniform(const uint8_t intensity) : _intensity(driver::EmitIntensity(intensity)), _phase(driver::Phase(0)) {}
  explicit Uniform(const driver::EmitIntensity intensity) : _intensity(intensity), _phase(driver::Phase(0)) {}

  AUTD3_DEF_PROP(driver::EmitIntensity, intensity)
  AUTD3_DEF_PARAM(Uniform, driver::Phase, phase)

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return native_methods::AUTDGainUniform(_intensity.value(), _phase.value());
  }
};
}  // namespace autd3::gain
