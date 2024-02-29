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
class Gain : public GainBase, public IntoDatagramWithSegment<native_methods::GainPtr, G>, public IntoGainCache<G>, public IntoGainTransform<G> {
 public:
  Gain() = default;                            // LCOV_EXCL_LINE
  Gain(const Gain& obj) = default;             // LCOV_EXCL_LINE
  Gain& operator=(const Gain& obj) = default;  // LCOV_EXCL_LINE
  Gain(Gain&& obj) = default;                  // LCOV_EXCL_LINE
  Gain& operator=(Gain&& obj) = default;       // LCOV_EXCL_LINE
  virtual ~Gain() = default;                   // LCOV_EXCL_LINE
};

}  // namespace autd3::driver
