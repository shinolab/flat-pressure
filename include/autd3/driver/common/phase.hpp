#pragma once

#include "autd3/native_methods.hpp"

namespace autd3::driver {

class Phase;

class UnitPhaseRad {
  AUTD3_API friend Phase operator*(double l, const UnitPhaseRad&);
};

constexpr UnitPhaseRad rad = UnitPhaseRad{};

class Phase final {
 public:
  AUTD3_API explicit Phase(const uint8_t value) : _value(value) {}

  AUTD3_API [[nodiscard]] static Phase from_rad(const double value) { return Phase(native_methods::AUTDPhaseFromRad(value)); }

  AUTD3_API [[nodiscard]] double radian() const { return native_methods::AUTDPhaseToRad(_value); }

  AUTD3_API [[nodiscard]] uint8_t value() const noexcept { return _value; }

  AUTD3_API friend Phase operator*(const double l, const UnitPhaseRad&) { return Phase::from_rad(l); }

  AUTD3_API auto operator<=>(const Phase&) const = default;

 private:
  uint8_t _value;
};

}  // namespace autd3::driver
