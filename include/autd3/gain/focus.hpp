#pragma once

#include <algorithm>

#include "autd3/def.hpp"
#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/cache.hpp"
#include "autd3/gain/transform.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain {

/**
 * @brief Gain to produce single focal point
 */
class Focus final : public driver::Gain<Focus>, public IntoCache<Focus>, public IntoTransform<Focus> {
 public:
  explicit Focus(driver::Vector3 p) : _pos(std::move(p)), _intensity(driver::EmitIntensity::maximum()), _phase_offset(driver::Phase(0)) {}

  AUTD3_DEF_PROP(driver::Vector3, pos)
  AUTD3_DEF_PARAM_INTENSITY(Focus, intensity)
  AUTD3_DEF_PROP(driver::Phase, phase_offset)

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return native_methods::AUTDGainFocus(_pos.x(), _pos.y(), _pos.z(), _intensity.value(), _phase_offset.value());
  }
};

}  // namespace autd3::gain
