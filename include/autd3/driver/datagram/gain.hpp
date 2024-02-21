#pragma once

#include <type_traits>

#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

class Gain {
 public:
  Gain() = default;
  Gain(const Gain& obj) = default;
  Gain& operator=(const Gain& obj) = default;
  Gain(Gain&& obj) = default;
  Gain& operator=(Gain&& obj) = default;
  virtual ~Gain() = default;  // LCOV_EXCL_LINE

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const { return AUTDGainIntoDatagram(gain_ptr(geometry)); }

  [[nodiscard]] virtual native_methods::GainPtr gain_ptr(const geometry::Geometry& geometry) const = 0;
};

template <class G>
concept gain = std::derived_from<std::remove_reference_t<G>, Gain>;

}  // namespace autd3::driver
