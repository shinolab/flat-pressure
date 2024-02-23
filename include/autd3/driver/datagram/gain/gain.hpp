#pragma once

#include <memory>
#include <type_traits>

#include "autd3/driver/datagram/gain/cache.hpp"
#include "autd3/driver/datagram/gain/transform.hpp"
#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <class G>
class Gain : public GainBase, public IntoDatagramWithSegment<native_methods::GainPtr, G>, public IntoCache<G>, public IntoTransform<G> {
 public:
  Gain() = default;
  Gain(const Gain& obj) = default;
  Gain& operator=(const Gain& obj) = default;
  Gain(Gain&& obj) = default;
  Gain& operator=(Gain&& obj) = default;
  virtual ~Gain() = default;  // LCOV_EXCL_LINE
};

}  // namespace autd3::driver
