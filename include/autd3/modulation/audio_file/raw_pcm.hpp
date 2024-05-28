#pragma once

#include <chrono>
#include <filesystem>

#include "autd3/driver/datagram/modulation/modulation.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::modulation::audio_file {

/**
 * @brief Modulation constructed from wav file
 * @details The wav data is re-sampled to the sampling frequency of Modulation.
 */
class RawPCM final : public driver::Modulation<RawPCM> {
 public:
  /**
   * @brief Constructor
   *
   * @param path Path to wav file
   * @param sample_rate Sampling frequency of raw pcm file
   */
  explicit RawPCM(std::filesystem::path path, const uint32_t sample_rate)
      : Modulation(driver::SamplingConfiguration::from_frequency(4e3)), _sample_rate(sample_rate), _path(std::move(path)) {}

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return validate(AUTDModulationRawPCM(_path.string().c_str(), _sample_rate, static_cast<native_methods::SamplingConfiguration>(_config),
                                         static_cast<native_methods::LoopBehavior>(_loop_behavior)));
  }

 private:
  uint32_t _sample_rate;
  std::filesystem::path _path;
};

}  // namespace autd3::modulation::audio_file
