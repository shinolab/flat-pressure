#pragma once

#include <concepts>

#include "autd3/driver/geometry/device.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <class F>
concept configure_reads_fpga_info_f = requires(F f, const geometry::Device& d) {
  { f(d) } -> std::same_as<bool>;
};

/**
 * @brief Datagram to configure force fan
 */
template <configure_reads_fpga_info_f F>
class ConfigureReadsFPGAState final {
  using native_f = bool (*)(const void*, native_methods::GeometryPtr, uint32_t);

 public:
  explicit ConfigureReadsFPGAState(F f) : _f(std::move(f)) {
    _f_native = +[](const void* context, const native_methods::GeometryPtr geometry_ptr, const uint32_t dev_idx) -> bool {
      const geometry::Device dev(dev_idx, AUTDDevice(geometry_ptr, dev_idx));
      return static_cast<const ConfigureReadsFPGAState*>(context)->_f(dev);
    };
  }

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const {
    return AUTDDatagramConfigureReadsFPGAState(const_cast<void*>(reinterpret_cast<const void*>(_f_native)),
                                               const_cast<void*>(static_cast<const void*>(this)), geometry.ptr());
  }

 private:
  F _f;
  native_f _f_native;
};

}  // namespace autd3::driver
