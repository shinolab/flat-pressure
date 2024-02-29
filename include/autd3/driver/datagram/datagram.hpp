#pragma once

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

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

}  // namespace autd3::driver
