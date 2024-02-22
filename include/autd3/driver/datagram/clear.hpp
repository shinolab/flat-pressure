#pragma once

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {
/**
 * @brief Datagram for clear all data in devices
 */
class Clear final {
 public:
  Clear() = default;

  AUTD3_API [[nodiscard]] static native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::AUTDDatagramClear(); }
};
}  // namespace autd3::driver
