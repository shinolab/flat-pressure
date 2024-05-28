#pragma once

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

/**
 * @brief Datagram to synchronize devices
 */
class Synchronize final {
 public:
  Synchronize() = default;

  [[nodiscard]] static native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::AUTDDatagramSynchronize(); }
};

}  // namespace autd3::driver
