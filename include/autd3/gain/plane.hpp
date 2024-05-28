#pragma once

#include <algorithm>

#include "autd3/def.hpp"
#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/gain/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain {

/**
 * @brief Gain to produce a plane wave
 */
class Plane final : public driver::Gain<Plane> {
 public:
  explicit Plane(driver::Vector3 d) : _dir(std::move(d)), _phase(driver::Phase(0)), _intensity(driver::EmitIntensity::maximum()) {}
  Plane() = delete;
  Plane(const Plane& obj) = default;
  Plane& operator=(const Plane& obj) = default;
  Plane(Plane&& obj) = default;
  Plane& operator=(Plane&& obj) = default;
  ~Plane() override = default;  // LCOV_EXCL_LINE

  AUTD3_DEF_PROP(driver::Vector3, dir)
  AUTD3_DEF_PARAM(Plane, driver::Phase, phase)
  AUTD3_DEF_PARAM_INTENSITY(Plane, intensity)

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return native_methods::AUTDGainPlane(_dir.x(), _dir.y(), _dir.z(), _intensity.value(), _phase.value());
  }
};

}  // namespace autd3::gain
