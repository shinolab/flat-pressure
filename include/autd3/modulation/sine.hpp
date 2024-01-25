#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/common/phase.hpp"
#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/modulation/transform.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::modulation {

class Fourier;

/**
 * @brief Sine wave modulation
 */
class Sine final : public driver::ModulationWithSamplingConfig<Sine>,
                   public IntoCache<Sine>,
                   public IntoTransform<Sine>,
                   public IntoRadiationPressure<Sine> {
 public:
  /**
   * @brief Constructor.
   * @details The sine wave is defined as `amp / 2 * sin(2π * freq * t + phase)
   * + offset`, where `t` is time, and `amp = 1`, `offset = 0.5` by default.
   *
   * @param freq Frequency of sine wave
   */
  explicit Sine(const double freq)
      : ModulationWithSamplingConfig(driver::SamplingConfiguration::from_frequency(4e3)),
        _freq(freq),
        _intensity(driver::EmitIntensity::maximum()),
        _offset(driver::EmitIntensity::maximum() / 2),
        _phase(driver::Phase(0)),
        _mode(native_methods::SamplingMode::ExactFrequency) {}

  AUTD3_DEF_PROP(double, freq)
  AUTD3_DEF_PARAM_INTENSITY(Sine, intensity)
  AUTD3_DEF_PARAM_INTENSITY(Sine, offset)
  AUTD3_DEF_PARAM(Sine, driver::Phase, phase)
  AUTD3_DEF_PARAM(Sine, native_methods::SamplingMode, mode)

  friend Fourier operator+(Sine&& lhs, const Sine& rhs);

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return AUTDModulationSine(_freq, static_cast<native_methods::SamplingConfiguration>(_config), _intensity.value(), _offset.value(), _phase.value(),
                              _mode);
  }
};

}  // namespace autd3::modulation
