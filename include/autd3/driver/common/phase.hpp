#pragma once

#include "autd3/native_methods.hpp"

namespace autd3::driver {

class Phase;

class UnitPhaseRad {
  friend Phase operator*(double l, const UnitPhaseRad&);
};

constexpr UnitPhaseRad rad = UnitPhaseRad{};

class Phase final {
 public:
  explicit Phase(const uint8_t value) : _value(value) {}

  [[nodiscard]] static Phase from_rad(const double value) { return Phase(native_methods::AUTDPhaseFromRad(value)); }

  [[nodiscard]] double radian() const { return native_methods::AUTDPhaseToRad(_value); }

  [[nodiscard]] uint8_t value() const noexcept { return _value; }

  auto operator<=>(const Phase&) const = default;

 private:
  uint8_t _value;
};

}  // namespace autd3::driver
