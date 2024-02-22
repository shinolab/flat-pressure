#pragma once

#include <chrono>
#include <filesystem>

#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/modulation/transform.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation::audio_file {

/**
 * @brief Modulation constructed from wav file
 * @details The wav data is re-sampled to the sampling frequency of Modulation.
 */
class Wav final : public driver::ModulationWithSamplingConfig<Wav>,
                  public IntoCache<Wav>,
                  public IntoRadiationPressure<Wav>,
                  public IntoTransform<Wav> {
 public:
  /**
   * @brief Constructor
   *
   * @param path Path to wav file
   */
  explicit Wav(std::filesystem::path path)
      : ModulationWithSamplingConfig(driver::SamplingConfiguration::from_frequency(4e3)), _path(std::move(path)) {}

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return validate(AUTDModulationWav(_path.string().c_str(), static_cast<native_methods::SamplingConfiguration>(_config),
                                      static_cast<native_methods::LoopBehavior>(_loop_behavior)));
  }

 private:
  std::filesystem::path _path;
};

}  // namespace autd3::modulation::audio_file
