#pragma once

#include <algorithm>

#include "autd3/def.hpp"
#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/gain/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain {

/**
 * @brief Gain to produce a Bessel beam
 */
class Bessel final : public driver::Gain<Bessel> {
 public:
  explicit Bessel(driver::Vector3 p, driver::Vector3 d, const double theta)
      : _pos(std::move(p)), _dir(std::move(d)), _theta(theta), _intensity(driver::EmitIntensity::maximum()), _phase_offset(driver::Phase(0)) {}
  Bessel() = delete;
  Bessel(const Bessel& obj) = default;
  Bessel& operator=(const Bessel& obj) = default;
  Bessel(Bessel&& obj) = default;
  Bessel& operator=(Bessel&& obj) = default;
  ~Bessel() override = default;  // LCOV_EXCL_LINE

  AUTD3_DEF_PROP(driver::Vector3, pos)
  AUTD3_DEF_PROP(driver::Vector3, dir)
  AUTD3_DEF_PROP(double, theta)
  AUTD3_DEF_PARAM_INTENSITY(Bessel, intensity)
  AUTD3_DEF_PROP(driver::Phase, phase_offset)

  AUTD3_API [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return native_methods::AUTDGainBessel(_pos.x(), _pos.y(), _pos.z(), _dir.x(), _dir.y(), _dir.z(), _theta, _intensity.value(),
                                          _phase_offset.value());
  }
};

}  // namespace autd3::gain
