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
 * @brief Gain to produce a Bessel beam
 */
class Bessel final : public driver::Gain, public IntoCache<Bessel>, public IntoTransform<Bessel> {
 public:
  explicit Bessel(driver::Vector3 p, driver::Vector3 d, const double theta)
      : _pos(std::move(p)), _dir(std::move(d)), _theta(theta), _intensity(driver::EmitIntensity::maximum()) {}

  AUTD3_DEF_PROP(driver::Vector3, pos)
  AUTD3_DEF_PROP(driver::Vector3, dir)
  AUTD3_DEF_PROP(double, theta)
  AUTD3_DEF_PARAM_INTENSITY(Bessel, intensity)

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return native_methods::AUTDGainBessel(_pos.x(), _pos.y(), _pos.z(), _dir.x(), _dir.y(), _dir.z(), _theta, _intensity.value());
  }
};

}  // namespace autd3::gain
