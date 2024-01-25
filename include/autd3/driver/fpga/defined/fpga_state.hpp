#pragma once

#include <cstdint>

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
  [[nodiscard]] bool is_thermal_assert() const noexcept { return (_info & 0x01) != 0; }

  [[nodiscard]] std::string to_string() const { return "Thermal assert = " + std::to_string(is_thermal_assert()); }

 private:
  uint8_t _info;
};

inline std::ostream& operator<<(std::ostream& os, const FPGAState& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace autd3::driver
