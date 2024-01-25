#pragma once

#include <chrono>

#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {
class STM {
 public:
  explicit STM(const std::optional<double> freq, const std::optional<std::chrono::nanoseconds> period,
               const std::optional<SamplingConfiguration> config)
      : _freq(freq), _period(period), _config(config) {}

  STM(const STM& obj) = default;
  STM& operator=(const STM& obj) = default;
  STM(STM&& obj) = default;
  STM& operator=(STM&& obj) = default;
  virtual ~STM() = default;  // LCOV_EXCL_LINE

  [[nodiscard]] std::optional<uint16_t> finish_idx() const {
    const auto idx = AUTDSTMPropsFinishIdx(props());
    return idx < 0 ? std::nullopt : std::optional(static_cast<uint16_t>(idx));
  }

  [[nodiscard]] std::optional<uint16_t> start_idx() const {
    const auto idx = AUTDSTMPropsStartIdx(props());
    return idx < 0 ? std::nullopt : std::optional(static_cast<uint16_t>(idx));
  }

 protected:
  [[nodiscard]] native_methods::STMPropsPtr props() const {
    native_methods::STMPropsPtr ptr{nullptr};
    if (_freq.has_value()) ptr = native_methods::AUTDSTMPropsFromFreq(_freq.value());
    if (_period.has_value()) ptr = native_methods::AUTDSTMPropsFromPeriod(static_cast<uint64_t>(_period.value().count()));
    if (_config.has_value()) ptr = AUTDSTMPropsFromSamplingConfig(static_cast<native_methods::SamplingConfiguration>(_config.value()));
    if (ptr._0 == nullptr) throw std::runtime_error("unreachable!");
    ptr = AUTDSTMPropsWithStartIdx(ptr, _start_idx);
    ptr = AUTDSTMPropsWithFinishIdx(ptr, _finish_idx);
    return ptr;
  }

  [[nodiscard]] double frequency_from_size(const size_t size) const { return AUTDSTMPropsFrequency(props(), size); }

  [[nodiscard]] SamplingConfiguration sampling_config_from_size(const size_t size) const {
    return SamplingConfiguration(validate(AUTDSTMPropsSamplingConfig(props(), size)));
  }

  [[nodiscard]] std::chrono::nanoseconds period_from_size(const size_t size) const {
    return std::chrono::nanoseconds(AUTDSTMPropsPeriod(props(), size));
  }

  std::optional<double> _freq;
  std::optional<std::chrono::nanoseconds> _period;
  std::optional<SamplingConfiguration> _config;
  int32_t _start_idx{-1};
  int32_t _finish_idx{-1};
};
}  // namespace autd3::driver
