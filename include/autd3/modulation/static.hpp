// File: static.hpp
// Project: modulation
// Created Date: 13/09/2023
// Author: Shun Suzuki
// -----
// Last Modified: 03/01/2024
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2023 Shun Suzuki. All rights reserved.
//

#pragma once

#include "autd3/internal/emit_intensity.hpp"
#include "autd3/internal/native_methods.hpp"
#include "autd3/internal/utils.hpp"
#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/modulation/transform.hpp"

namespace autd3::modulation {

/**
 * @brief Without modulation
 */
class Static final : public internal::Modulation, public IntoCache<Static>, public IntoRadiationPressure<Static>, public IntoTransform<Static> {
 public:
  Static() = default;
  explicit Static(const uint8_t intensity) : _intensity(internal::EmitIntensity(intensity)) {}
  explicit Static(const internal::EmitIntensity intensity) : _intensity(intensity) {}

  static Static with_intensity(const uint8_t intensity) { return Static(intensity); }
  static Static with_intensity(const internal::EmitIntensity intensity) { return Static(intensity); }

  [[nodiscard]] internal::native_methods::ModulationPtr modulation_ptr() const override {
    if (_intensity.has_value())
      return internal::native_methods::AUTDModulationStaticWithIntensity(_intensity.value().value());
    else
      return internal::native_methods::AUTDModulationStatic();
  }

 private:
  std::optional<internal::EmitIntensity> _intensity;
};
}  // namespace autd3::modulation
