#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/common/phase.hpp"

namespace autd3::driver {
struct Drive final {
  Phase phase;
  EmitIntensity intensity;

  explicit Drive(const Phase phase, const EmitIntensity intensity) : phase(phase), intensity(intensity) {}
  explicit Drive(const Phase phase, const uint8_t intensity) : phase(phase), intensity(intensity) {}

  auto operator<=>(const Drive&) const = default;
};

}  // namespace autd3::driver
