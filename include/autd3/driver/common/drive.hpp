#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/common/phase.hpp"

namespace autd3::driver {
struct Drive final {
  Phase phase;
  EmitIntensity intensity;

  AUTD3_API explicit Drive(const Phase phase, const EmitIntensity intensity) : phase(phase), intensity(intensity) {}
  AUTD3_API explicit Drive(const Phase phase, const uint8_t intensity) : phase(phase), intensity(intensity) {}

  AUTD3_API auto operator<=>(const Drive&) const = default;
};

}  // namespace autd3::driver
