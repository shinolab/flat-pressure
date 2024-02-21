#pragma once

#include <chrono>
#include <ranges>

#include "autd3/def.hpp"
#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/driver/datagram/datagram.hpp"
#include "autd3/driver/datagram/stm/stm.hpp"
#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {
/**
 * @brief Control point for FocusSTM
 */
struct ControlPoint {
  /**
   * @brief Focus point
   */
  Vector3 point;
  /**
   * @brief Duty shift
   * @details Duty ratio of ultrasound will be `50% >> duty_shift`. If
   * `duty_shift` is 0, duty ratio is 50%, which means the amplitude is the
   * maximum.
   */
  EmitIntensity intensity;
};

template <class R>
concept focus_range_v = std::ranges::viewable_range<R> && std::same_as<std::ranges::range_value_t<R>, Vector3>;

template <class R>
concept focus_range_c = std::ranges::viewable_range<R> && std::same_as<std::ranges::range_value_t<R>, ControlPoint>;

/**
 * @brief FocusSTM is an STM for moving Gain.
 * @details The sampling timing is determined by hardware, thus the sampling
 * time is precise. FocusSTM has following restrictions:
 * - The maximum number of sampling points is 65536.
 * - The sampling frequency is
 * [autd3::native_methods::FPGA_CLK_FREQ]/N, where `N` is a 32-bit
 * unsigned integer and must be at 4096.
 */
class FocusSTM final : public STM, public IntoDatagramWithSegment<native_methods::FocusSTMPtr, FocusSTM> {
 public:
  FocusSTM() = delete;
  FocusSTM(const FocusSTM& obj) = default;
  FocusSTM& operator=(const FocusSTM& obj) = default;
  FocusSTM(FocusSTM&& obj) = default;
  FocusSTM& operator=(FocusSTM&& obj) = default;
  ~FocusSTM() override = default;  // LCOV_EXCL_LINE

  static FocusSTM from_freq(const double freq) { return FocusSTM(freq, std::nullopt, std::nullopt); }

  static FocusSTM from_sampling_config(const SamplingConfiguration config) { return FocusSTM(std::nullopt, std::nullopt, config); }

  template <typename Rep, typename Period>
  static FocusSTM from_period(const std::chrono::duration<Rep, Period> period) {
    return FocusSTM(std::nullopt, std::chrono::duration_cast<std::chrono::nanoseconds>(period), std::nullopt);
  }

  [[nodiscard]] native_methods::FocusSTMPtr raw_ptr(const geometry::Geometry&) const override {
    return validate(AUTDSTMFocus(props(), reinterpret_cast<const double*>(_points.data()), reinterpret_cast<const uint8_t*>(_intensities.data()),
                                 _intensities.size()));
  }

  [[nodiscard]] native_methods::DatagramPtr into_segment(const native_methods::FocusSTMPtr p, const native_methods::Segment segment,
                                                         const bool update_segment) const override {
    return AUTDSTMFocusIntoDatagramWithSegment(p, segment, update_segment);
  }

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const { return AUTDSTMFocusIntoDatagram(raw_ptr(geometry)); }

  /**
   * @brief Add focus point
   *
   * @param point Focus point
   * @param intensity Emission intensity
   * @return FocusSTM
   */
  void add_focus(Vector3 point, const EmitIntensity intensity = EmitIntensity::maximum()) & {
    _points.emplace_back(std::move(point));
    _intensities.emplace_back(intensity);
  }

  /**
   * @brief Add focus point
   *
   * @param point Focus point
   * @param intensity Emission intensity
   * @return FocusSTM
   */
  [[nodiscard]] FocusSTM&& add_focus(Vector3 point, const EmitIntensity intensity = EmitIntensity::maximum()) && {
    _points.emplace_back(std::move(point));
    _intensities.emplace_back(intensity);
    return std::move(*this);
  }

  /**
   * @brief Add ControlPoint
   *
   * @param p control point
   * @return FocusSTM
   */
  void add_focus(ControlPoint p) & {
    _points.emplace_back(std::move(p.point));
    _intensities.emplace_back(p.intensity);
  }

  /**
   * @brief Add ControlPoint
   *
   * @param p control point
   * @return FocusSTM
   */
  [[nodiscard]] FocusSTM&& add_focus(ControlPoint p) && {
    _points.emplace_back(std::move(p.point));
    _intensities.emplace_back(p.intensity);
    return std::move(*this);
  }

  /**
   * @brief Add foci
   *
   * @tparam R
   * @param iter iterator of focus points
   */
  template <focus_range_v R>
  void add_foci_from_iter(R&& iter) & {
    for (Vector3 e : iter) {
      _points.emplace_back(std::move(e));
      _intensities.emplace_back(EmitIntensity::maximum());
    }
  }

  /**
   * @brief Add foci
   *
   * @tparam R
   * @param iter iterator of focus points
   */
  template <focus_range_v R>
  [[nodiscard]] FocusSTM add_foci_from_iter(R&& iter) && {
    for (Vector3 e : iter) {
      _points.emplace_back(std::move(e));
      _intensities.emplace_back(EmitIntensity::maximum());
    }
    return std::move(*this);
  }

  /**
   * @brief Add foci
   *
   * @tparam R
   * @param iter iterator of [ControlPoint]s
   */
  template <focus_range_c R>
  void add_foci_from_iter(R&& iter) & {
    for (ControlPoint e : iter) {
      _points.emplace_back(std::move(e.point));
      _intensities.emplace_back(e.intensity);
    }
  }

  /**
   * @brief Add foci
   *
   * @tparam R
   * @param iter iterator of [ControlPoint]s
   */
  template <focus_range_c R>
  [[nodiscard]] FocusSTM add_foci_from_iter(R&& iter) && {
    for (ControlPoint e : iter) {
      _points.emplace_back(std::move(e.point));
      _intensities.emplace_back(e.intensity);
    }
    return std::move(*this);
  }

  [[nodiscard]] double frequency() const { return frequency_from_size(_points.size()); }
  [[nodiscard]] std::chrono::nanoseconds period() const { return period_from_size(_points.size()); }
  [[nodiscard]] SamplingConfiguration sampling_config() const { return sampling_config_from_size(_points.size()); }

 private:
  explicit FocusSTM(const std::optional<double> freq, const std::optional<std::chrono::nanoseconds> period,
                    const std::optional<SamplingConfiguration> config)
      : STM(freq, period, config) {}

  std::vector<Vector3> _points;
  std::vector<EmitIntensity> _intensities;
};
}  // namespace autd3::driver
