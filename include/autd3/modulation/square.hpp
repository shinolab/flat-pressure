

#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/modulation/transform.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::modulation {

/**
 * @brief Square wave modulation
 */
class Square final : public driver::ModulationWithSamplingConfig<Square>,
                     public IntoCache<Square>,
                     public IntoTransform<Square>,
                     public IntoRadiationPressure<Square> {
 public:
  /**
   * @brief Constructor
   *
   * @param freq Frequency of square wave
   */
  explicit Square(const double freq)
      : ModulationWithSamplingConfig(driver::SamplingConfiguration::from_frequency(4e3)),
        _freq(freq),
        _low(driver::EmitIntensity::minimum()),
        _high(driver::EmitIntensity::maximum()),
        _duty(0.5),
        _mode(native_methods::SamplingMode::ExactFrequency) {}

  AUTD3_DEF_PROP(double, freq)
  AUTD3_DEF_PARAM_INTENSITY(Square, low)
  AUTD3_DEF_PARAM_INTENSITY(Square, high)
  AUTD3_DEF_PARAM(Square, double, duty)
  AUTD3_DEF_PARAM(Square, native_methods::SamplingMode, mode)

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return AUTDModulationSquare(_freq, static_cast<native_methods::SamplingConfiguration>(_config), _low.value(), _high.value(), _duty, _mode,
                                static_cast<native_methods::LoopBehavior>(_loop_behavior));
  }
};

}  // namespace autd3::modulation
