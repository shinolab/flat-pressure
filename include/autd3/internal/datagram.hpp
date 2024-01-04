// File: datagram.hpp
// Project: internal
// Created Date: 29/05/2023
// Author: Shun Suzuki
// -----
// Last Modified: 04/01/2024
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2023 Shun Suzuki. All rights reserved.
//

#pragma once

#include "autd3/internal/geometry/geometry.hpp"
#include "autd3/internal/native_methods.hpp"
#include "autd3/internal/utils.hpp"

namespace autd3::internal {

template <class D>
concept datagram = requires(D d, const geometry::Geometry& g) {
  { d.ptr(g) } -> std::same_as<native_methods::DatagramPtr>;
};

class NullDatagram final {
 public:
  NullDatagram() = default;
  ~NullDatagram() = default;
  NullDatagram(const NullDatagram& v) noexcept = default;
  NullDatagram& operator=(const NullDatagram& obj) = default;
  NullDatagram(NullDatagram&& obj) = default;
  NullDatagram& operator=(NullDatagram&& obj) = default;

  [[nodiscard]] static native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::DatagramPtr{nullptr}; }
};

/**
 * @brief Datagram to configure silencer
 */
class ConfigureSilencer final {
 public:
  /**
   * @brief Datagram to configure silencer
   */
  class FixedUpdateRate final {
   public:
    explicit FixedUpdateRate(const uint16_t update_rate_intensity, const uint16_t update_rate_phase) noexcept
        : _value_intensity(update_rate_intensity), _value_phase(update_rate_phase) {}

    [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) const {
      return validate(native_methods::AUTDDatagramSilencerFixedUpdateRate(_value_intensity, _value_phase));
    }

   private:
    uint16_t _value_intensity;
    uint16_t _value_phase;
  };

  /**
   * @brief Datagram to configure silencer
   */
  class FixedCompletionSteps final {
   public:
    explicit FixedCompletionSteps(const uint16_t steps_intensity, const uint16_t steps_phase) noexcept
        : _value_intensity(steps_intensity), _value_phase(steps_phase) {}

    AUTD3_DEF_PARAM(FixedCompletionSteps, bool, strict_mode)

    [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) const {
      auto ptr = validate(native_methods::AUTDDatagramSilencerFixedCompletionSteps(_value_intensity, _value_phase));
      if (_strict_mode.has_value()) ptr = native_methods::AUTDDatagramSilencerFixedCompletionStepsWithStrictMode(ptr, _strict_mode.value());
      return ptr;
    }

   private:
    uint16_t _value_intensity;
    uint16_t _value_phase;
    std::optional<bool> _strict_mode;
  };

  ConfigureSilencer() = default;

  /**
   * @brief Constructor
   *
   * @param update_rate_intensity Intensity update rate of silencer. The smaller value
   * is, the quieter the output is.
   * @param update_rate_phase Phase update rate of silencer. The smaller value is, the
   * quieter the output is.
   */
  static FixedUpdateRate fixed_update_rate(const uint16_t update_rate_intensity, const uint16_t update_rate_phase) noexcept {
    return FixedUpdateRate(update_rate_intensity, update_rate_phase);
  }

  /**
   * @brief Constructor
   *
   * @param steps_intensity Intensity completion steps of silencer. The smaller value is,
   * the quieter the output is.
   * @param steps_phase Phase completion steps of silencer. The smaller value is, the
   * quieter the output is.
   */
  static FixedCompletionSteps fixed_completion_steps(const uint16_t steps_intensity, const uint16_t steps_phase) noexcept {
    return FixedCompletionSteps(steps_intensity, steps_phase);
  }

  /**
   * @brief Disable silencer
   */
  static FixedCompletionSteps disable() noexcept { return fixed_completion_steps(1, 1); }

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) const {
    return native_methods::AUTDDatagramSilencerFixedCompletionStepsWithStrictMode(
        validate(native_methods::AUTDDatagramSilencerFixedCompletionSteps(10, 40)), true);
  }
};

/**
 * @brief Datagram for clear all data in devices
 */
class Clear final {
 public:
  Clear() = default;

  [[nodiscard]] static native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::AUTDDatagramClear(); }
};

/**
 * @brief Datagram to synchronize devices
 */
class Synchronize final {
 public:
  Synchronize() = default;

  [[nodiscard]] static native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::AUTDDatagramSynchronize(); }
};

}  // namespace autd3::internal
