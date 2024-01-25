#pragma once

#include <numbers>

#include "autd3/def.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver::geometry {

class Angle;

class UnitRad {
  friend Angle operator*(double l, const UnitRad&);
};
class UnitDegree {
  friend Angle operator*(double l, const UnitDegree&);
};

constexpr UnitRad rad = UnitRad{};
constexpr UnitDegree deg = UnitDegree{};

class Angle {
 public:
  friend class UnitRad;
  friend class UnitDegree;

  [[nodiscard]] double to_radian() const { return _value; }

  friend Angle operator*(const double l, const UnitRad&) { return Angle(l); }
  friend Angle operator*(const double l, const UnitDegree&) { return Angle(l / 180 * std::numbers::pi); }

 private:
  explicit Angle(const double value) : _value(value) {}

  double _value;
};

class EulerAngles {
 public:
  static Quaternion from_zyz(const Angle& z1, const Angle& y, const Angle& z2) {
    double v[4];
    native_methods::AUTDRotationFromEulerZYZ(z1.to_radian(), y.to_radian(), z2.to_radian(), v);
    return {v[0], v[1], v[2], v[3]};
  }
};

}  // namespace autd3::driver::geometry
