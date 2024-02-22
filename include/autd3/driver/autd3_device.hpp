#pragma once

#include <algorithm>
#include <optional>

#include "autd3/def.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

class AUTD3 {
 public:
  /**
   * @brief Number of transducer in an AUTD3 device
   */
  static constexpr size_t NUM_TRANS_IN_UNIT = native_methods::NUM_TRANS_IN_UNIT;
  /**
   * @brief Number of transducer in x-axis of AUTD3 device
   */
  static constexpr size_t NUM_TRANS_IN_X = native_methods::NUM_TRANS_IN_X;
  /**
   * @brief Number of transducer in y-axis of AUTD3 device
   */
  static constexpr size_t NUM_TRANS_IN_Y = native_methods::NUM_TRANS_IN_Y;
  /**
   * @brief Spacing between transducers
   */
  static constexpr double TRANS_SPACING = native_methods::TRANS_SPACING_MM;
  /**
   * @brief Device width including substrate
   */
  static constexpr double DEVICE_WIDTH = native_methods::DEVICE_WIDTH_MM;
  /**
   * @brief Device height including substrate
   */
  static constexpr double DEVICE_HEIGHT = native_methods::DEVICE_HEIGHT_MM;

  /**
   * @brief FPGA clock frequency
   */
  static constexpr double FPGA_CLK_FREQ = native_methods::FPGA_CLK_FREQ;

  /**
   * @brief Constructor
   *
   * @param pos Global position
   */
  explicit AUTD3(Vector3 pos) : _pos(std::move(pos)) {}

  /**
   * @brief Set device rotation
   *
   * @param rot Rotation
   */
  AUTD3_API void with_rotation(const Quaternion& rot) & { _rot = rot; }

  /**
   * @brief Set device rotation
   *
   * @param rot Rotation
   */
  AUTD3_API [[nodiscard]] AUTD3 with_rotation(const Quaternion& rot) && {
    _rot = rot;
    return std::move(*this);
  }

  AUTD3_API [[nodiscard]] Vector3 position() const { return _pos; }
  AUTD3_API [[nodiscard]] std::optional<Quaternion> rotation() const { return _rot; }

 private:
  Vector3 _pos{};
  std::optional<Quaternion> _rot{std::nullopt};
};
}  // namespace autd3::driver
