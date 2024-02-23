#pragma once

#include <chrono>
#include <memory>
#include <ranges>

#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/driver/datagram/datagram.hpp"
#include "autd3/driver/datagram/gain/base.hpp"
#include "autd3/driver/datagram/stm/stm.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {
template <class R>
concept gain_range = std::ranges::viewable_range<R> && gain<std::ranges::range_value_t<R>>;

/**
 * @brief GainSTM is an STM for moving Gain.
 * @details The sampling timing is determined by hardware, thus the sampling
 * time is precise. GainSTM has following restrictions:
 * - The maximum number of sampling Gain is 1024.
 * - The sampling frequency is
 * [autd3::native_methods::FPGA_CLK_FREQ]/N, where `N` is a 32-bit
 * unsigned integer and must be at least 512.
 */
class GainSTM final : public STM, public DatagramS<native_methods::GainSTMPtr> {
 public:
  GainSTM() = delete;
  GainSTM(const GainSTM& obj) = default;
  GainSTM& operator=(const GainSTM& obj) = default;
  GainSTM(GainSTM&& obj) = default;
  GainSTM& operator=(GainSTM&& obj) = default;
  ~GainSTM() override = default;  // LCOV_EXCL_LINE

  /**
   * @brief Constructor
   *
   * @param freq STM frequency
   */
  AUTD3_API [[nodiscard]] static GainSTM from_freq(const double freq) { return GainSTM(freq, std::nullopt, std::nullopt); }

  /**
   * @brief Constructor
   *
   * @param config Sampling configuration
   * @return GainSTM
   */
  AUTD3_API [[nodiscard]] static GainSTM from_sampling_config(const SamplingConfiguration config) {
    return GainSTM(std::nullopt, std::nullopt, config);
  }

  template <typename Rep, typename Period>
  AUTD3_API [[nodiscard]] static GainSTM from_period(const std::chrono::duration<Rep, Period> period) {
    return GainSTM(std::nullopt, std::nullopt, std::nullopt, std::chrono::duration_cast<std::chrono::nanoseconds>(period));
  }

  AUTD3_API [[nodiscard]] native_methods::GainSTMPtr raw_ptr(const geometry::Geometry& geometry) const override {
    const auto mode = _mode.has_value() ? _mode.value() : native_methods::GainSTMMode::PhaseIntensityFull;
    std::vector<native_methods::GainPtr> gains;
    gains.reserve(_gains.size());
    std::ranges::transform(_gains, std::back_inserter(gains), [&](const auto& gain) { return gain->gain_ptr(geometry); });
    return validate(AUTDSTMGain(props(), gains.data(), static_cast<uint32_t>(gains.size()), mode));
  }

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr into_segment(const native_methods::GainSTMPtr p, const native_methods::Segment segment,
                                                                   const bool update_segment) const override {
    return AUTDSTMGainIntoDatagramWithSegment(p, segment, update_segment);
  }

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const {
    return AUTDSTMGainIntoDatagram(raw_ptr(geometry));
  }

  AUTD3_API [[nodiscard]] DatagramWithSegment<native_methods::GainSTMPtr> with_segment(const native_methods::Segment segment,
                                                                                       const bool update_segment) {
    return DatagramWithSegment<native_methods::GainSTMPtr>(std::make_unique<GainSTM>(std::move(*this)), segment, update_segment);
  }

  /**
   * @brief Add Gain to the GainSTM
   *
   * @tparam G Gain
   * @param gain gain
   * @return GainSTM
   */
  template <gain G>
  AUTD3_API void add_gain(G&& gain) & {
    _gains.emplace_back(std::make_shared<std::remove_reference_t<G>>(std::forward<G>(gain)));
  }

  /**
   * @brief Add Gain to the GainSTM
   *
   * @tparam G Gain
   * @param gain gain
   * @return GainSTM
   */
  template <gain G>
  AUTD3_API [[nodiscard]] GainSTM&& add_gain(G&& gain) && {
    _gains.emplace_back(std::make_shared<std::remove_reference_t<G>>(std::forward<G>(gain)));
    return std::move(*this);
  }

  /**
   * @brief Add Gains to the GainSTM
   *
   * @tparam R Iterator
   * @param iter gain iterator
   */
  template <gain_range R>
  AUTD3_API void add_gains_from_iter(R&& iter) & {
    for (auto e : iter)
      _gains.emplace_back(std::make_shared<std::remove_reference_t<std::ranges::range_value_t<R>>>(std::forward<std::ranges::range_value_t<R>>(e)));
  }

  /**
   * @brief Add Gains to the GainSTM
   *
   * @tparam R Iterator
   * @param iter gain iterator
   * @return GainSTM
   */
  template <gain_range R>
  AUTD3_API [[nodiscard]] GainSTM add_gains_from_iter(R&& iter) && {
    for (auto e : iter)
      _gains.emplace_back(std::make_shared<std::remove_reference_t<std::ranges::range_value_t<R>>>(std::forward<std::ranges::range_value_t<R>>(e)));
    return std::move(*this);
  }

  AUTD3_API [[nodiscard]] double frequency() const { return frequency_from_size(_gains.size()); }
  AUTD3_API [[nodiscard]] std::chrono::nanoseconds period() const { return period_from_size(_gains.size()); }
  AUTD3_API [[nodiscard]] SamplingConfiguration sampling_config() const { return sampling_config_from_size(_gains.size()); }

  AUTD3_API void with_mode(const native_methods::GainSTMMode mode) & { _mode = mode; }
  AUTD3_API [[nodiscard]] GainSTM&& with_mode(const native_methods::GainSTMMode mode) && {
    _mode = mode;
    return std::move(*this);
  }

 private:
  AUTD3_API explicit GainSTM(const std::optional<double> freq, const std::optional<std::chrono::nanoseconds> period,
                             const std::optional<SamplingConfiguration> config)
      : STM(freq, period, config) {}

  std::vector<std::shared_ptr<GainBase>> _gains;
  std::optional<native_methods::GainSTMMode> _mode;
};

class ChangeGainSTMSegment final {
 public:
  AUTD3_API explicit ChangeGainSTMSegment(const native_methods::Segment segment) : _segment(segment){};

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) {
    return native_methods::AUTDDatagramChangeGainSTMSegment(_segment);
  }

 private:
  native_methods::Segment _segment;
};

}  // namespace autd3::driver
