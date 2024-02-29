

#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/modulation/modulation.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::modulation {

/**
 * @brief Square wave modulation
 */
class Square final : public driver::Modulation<Square> {
 public:
  /**
   * @brief Constructor
   *
   * @param freq Frequency of square wave
   */
  explicit Square(const double freq)
      : Modulation(driver::SamplingConfiguration::from_frequency(4e3)),
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

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return AUTDModulationSquare(_freq, static_cast<native_methods::SamplingConfiguration>(_config), _low.value(), _high.value(), _duty, _mode,
                                static_cast<native_methods::LoopBehavior>(_loop_behavior));
  }
};

}  // namespace autd3::modulation
