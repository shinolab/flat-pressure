#pragma once

#include <concepts>

#include "autd3/driver/geometry/device.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/driver/geometry/transducer.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <class F>
concept configure_mod_delay_f = requires(F f, const geometry::Device& d, const geometry::Transducer& tr) {
  { f(d, tr) } -> std::same_as<uint16_t>;
};

/**
 * @brief Datagram to set modulation delay
 */
template <configure_mod_delay_f F>
class ConfigureModDelay final {
  using native_f = uint16_t (*)(const void*, native_methods::GeometryPtr, uint32_t, uint8_t);

 public:
  explicit ConfigureModDelay(F f) : _f(std::move(f)) {
    _f_native = +[](const void* context, const native_methods::GeometryPtr geometry_ptr, const uint32_t dev_idx, const uint8_t tr_idx) -> uint16_t {
      const geometry::Device dev(dev_idx, AUTDDevice(geometry_ptr, dev_idx));
      const geometry::Transducer tr(static_cast<size_t>(tr_idx), dev.ptr());
      return static_cast<const ConfigureModDelay*>(context)->_f(dev, tr);
    };
  }

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const {
    return AUTDDatagramConfigureModDelay(const_cast<void*>(reinterpret_cast<const void*>(_f_native)),
                                         const_cast<void*>(static_cast<const void*>(this)), geometry.ptr());
  }

 private:
  F _f;
  native_f _f_native;
};

}  // namespace autd3::driver
