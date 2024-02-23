#pragma once

#include <type_traits>

#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

class GainBase : public DatagramS<native_methods::GainPtr> {
 public:
  GainBase() = default;
  GainBase(const GainBase& obj) = default;
  GainBase& operator=(const GainBase& obj) = default;
  GainBase(GainBase&& obj) = default;
  GainBase& operator=(GainBase&& obj) = default;
  virtual ~GainBase() = default;  // LCOV_EXCL_LINE

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const {
    return AUTDGainIntoDatagram(gain_ptr(geometry));
  }

  AUTD3_API [[nodiscard]] native_methods::GainPtr raw_ptr(const geometry::Geometry& geometry) const override { return gain_ptr(geometry); }

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr into_segment(const native_methods::GainPtr p, const native_methods::Segment segment,
                                                                   const bool update_segment) const override {
    return AUTDGainIntoDatagramWithSegment(p, segment, update_segment);
  }

  AUTD3_API [[nodiscard]] virtual native_methods::GainPtr gain_ptr(const geometry::Geometry&) const = 0;
};

template <class G>
concept gain = std::derived_from<std::remove_reference_t<G>, GainBase>;

class ChangeGainSegment final {
 public:
  explicit ChangeGainSegment(const native_methods::Segment segment) : _segment(segment){};

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) {
    return native_methods::AUTDDatagramChangeGainSegment(_segment);
  }

 private:
  native_methods::Segment _segment;
};

}  // namespace autd3::driver
