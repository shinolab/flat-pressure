#pragma once

#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::driver {
class STM;
class Modulation;

class SamplingConfiguration final {
 public:
  friend class STM;
  friend class Modulation;

  [[nodiscard]] static SamplingConfiguration from_frequency(const double f) {
    return SamplingConfiguration(validate(native_methods::AUTDSamplingConfigFromFrequency(f)));
  }

  [[nodiscard]] static SamplingConfiguration from_frequency_division(const uint32_t div) {
    return SamplingConfiguration(validate(native_methods::AUTDSamplingConfigFromFrequencyDivision(div)));
  }
  template <typename Rep, typename Period>
  [[nodiscard]] static SamplingConfiguration from_period(const std::chrono::duration<Rep, Period> period) {
    return SamplingConfiguration(validate(
        native_methods::AUTDSamplingConfigFromPeriod(static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(period).count()))));
  }

  [[nodiscard]] double frequency() const { return AUTDSamplingConfigFrequency(_internal); }

  [[nodiscard]] uint32_t frequency_division() const { return AUTDSamplingConfigFrequencyDivision(_internal); }

  [[nodiscard]] std::chrono::nanoseconds period() const { return std::chrono::nanoseconds(AUTDSamplingConfigPeriod(_internal)); }

  explicit operator native_methods::SamplingConfiguration() const { return _internal; }

 private:
  explicit SamplingConfiguration(const native_methods::SamplingConfiguration internal_) : _internal(internal_) {}

  native_methods::SamplingConfiguration _internal;
};

}  // namespace autd3::driver
