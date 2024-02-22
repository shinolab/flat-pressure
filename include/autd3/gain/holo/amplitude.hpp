#pragma once

#include "autd3/native_methods.hpp"

namespace autd3::gain::holo {
class Amplitude;

class UnitPascal {
  friend Amplitude operator*(double l, const UnitPascal&);
};
class UnitSPL {
  friend Amplitude operator*(double l, const UnitSPL&);
};

constexpr UnitPascal Pascal = UnitPascal{};
constexpr UnitSPL dB = UnitSPL{};

class Amplitude {
 public:
  friend class UnitPascal;
  friend class UnitSPL;

  AUTD3_API [[nodiscard]] double as_pascal() const { return _value; }
  AUTD3_API [[nodiscard]] double as_spl() const { return native_methods::AUTDGainHoloPascalToSPL(_value); }

  AUTD3_API friend Amplitude operator*(const double l, const UnitPascal&) { return Amplitude(l); }
  AUTD3_API friend Amplitude operator*(const double l, const UnitSPL&) { return Amplitude(native_methods::AUTDGainHoloSPLToPascal(l)); }

 private:
  explicit Amplitude(const double value) : _value(value) {}

  double _value;
};

}  // namespace autd3::gain::holo