#pragma once

#include <concepts>

#include "autd3/driver/geometry/device.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/driver/geometry/transducer.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <class F>
concept configure_debug_output_idx_f = requires(F f, const geometry::Device& d) {
  { f(d) } -> std::same_as<const geometry::Transducer*>;
};

/**
 * @brief Datagram to configure debug output
 */
template <configure_debug_output_idx_f F>
class ConfigureDebugOutputIdx final {
  using native_f = uint8_t (*)(const void*, native_methods::GeometryPtr, uint32_t);

 public:
  explicit ConfigureDebugOutputIdx(F f) : _f(std::move(f)) {
    _f_native = +[](const void* context, const native_methods::GeometryPtr geometry_ptr, const uint32_t dev_idx) -> uint8_t {
      const geometry::Device dev(dev_idx, AUTDDevice(geometry_ptr, dev_idx));
      const auto* tr = static_cast<const ConfigureDebugOutputIdx*>(context)->_f(dev);
      return tr != nullptr ? static_cast<uint8_t>(tr->idx()) : 0xFF;
    };
  }

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const {
    return AUTDDatagramConfigureDebugOutputIdx(const_cast<void*>(reinterpret_cast<const void*>(_f_native)),
                                               const_cast<void*>(static_cast<const void*>(this)), geometry.ptr());
  }

 private:
  F _f;
  native_f _f_native;
};

}  // namespace autd3::driver
