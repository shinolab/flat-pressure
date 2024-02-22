#pragma once

#include "autd3/driver/datagram/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/cache.hpp"
#include "autd3/gain/transform.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain {

/**
 * @brief Gain to output nothing
 */
class Null final : public driver::Gain<Null>, public IntoCache<Null>, public IntoTransform<Null> {
 public:
  Null() = default;
  Null(const Null& obj) = default;
  Null& operator=(const Null& obj) = default;
  Null(Null&& obj) = default;
  Null& operator=(Null&& obj) = default;
  ~Null() override = default;  // LCOV_EXCL_LINE

  AUTD3_API [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return native_methods::AUTDGainNull();
  }
};

}  // namespace autd3::gain
