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
class Null final : public driver::Gain, public IntoCache<Null>, public IntoTransform<Null> {
 public:
  Null() = default;

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override { return native_methods::AUTDGainNull(); }
};

}  // namespace autd3::gain
