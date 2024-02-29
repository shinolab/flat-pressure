#pragma once

#include <cstdint>
#include <optional>

#include "autd3/native_methods.hpp"

namespace autd3::driver {

class FPGAState {
 public:
  explicit FPGAState(const uint8_t info) noexcept : _info(info) {}
  ~FPGAState() = default;
  FPGAState(const FPGAState& v) = default;
  FPGAState& operator=(const FPGAState& obj) = default;
  FPGAState(FPGAState&& obj) = default;
  FPGAState& operator=(FPGAState&& obj) = default;

  /**
   * @brief Check if thermal sensor is asserted
   */
  [[nodiscard]] bool is_thermal_assert() const noexcept { return (_info & (1 << 0)) != 0; }

  [[nodiscard]] native_methods::Segment current_mod_segment() const noexcept {
    switch (_info & (1 << 1)) {
      case 0:
        return native_methods::Segment::S0;
      default:
        return native_methods::Segment::S1;
    }
  }

  [[nodiscard]] std::optional<native_methods::Segment> current_stm_segment() const noexcept {
    if (!is_stm_mode()) {
      return std::nullopt;
    }
    switch (_info & (1 << 2)) {
      case 0:
        return native_methods::Segment::S0;
      default:
        return native_methods::Segment::S1;
    }
  }

  [[nodiscard]] std::optional<native_methods::Segment> current_gain_segment() const noexcept {
    if (!is_gain_mode()) {
      return std::nullopt;
    }
    switch (_info & (1 << 2)) {
      case 0:
        return native_methods::Segment::S0;
      default:
        return native_methods::Segment::S1;
    }
  }

  [[nodiscard]] bool is_gain_mode() const noexcept { return (_info & (1 << 3)) != 0; }

  [[nodiscard]] bool is_stm_mode() const noexcept { return !is_gain_mode(); }
  // LCOV_EXCL_START
 private:
  uint8_t _info;
  // LCOV_EXCL_STOP
};

}  // namespace autd3::driver
