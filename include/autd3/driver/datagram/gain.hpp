#pragma once

#include <memory>
#include <type_traits>

#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

class IGain {
 public:
  IGain() = default;
  IGain(const IGain& obj) = default;
  IGain& operator=(const IGain& obj) = default;
  IGain(IGain&& obj) = default;
  IGain& operator=(IGain&& obj) = default;
  virtual ~IGain() = default;  // LCOV_EXCL_LINE

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const { return AUTDGainIntoDatagram(gain_ptr(geometry)); }

  [[nodiscard]] virtual native_methods::GainPtr gain_ptr(const geometry::Geometry&) const = 0;
};

template <class G>
class Gain : public IGain, public DatagramS<native_methods::GainPtr> {
 public:
  Gain() = default;
  Gain(const Gain& obj) = default;
  Gain& operator=(const Gain& obj) = default;
  Gain(Gain&& obj) = default;
  Gain& operator=(Gain&& obj) = default;
  virtual ~Gain() = default;  // LCOV_EXCL_LINE

  [[nodiscard]] native_methods::GainPtr raw_ptr(const geometry::Geometry& geometry) const override { return gain_ptr(geometry); }

  [[nodiscard]] native_methods::DatagramPtr into_segment(const native_methods::GainPtr p, const native_methods::Segment segment,
                                                         const bool update_segment) const override {
    return AUTDGainIntoDatagramWithSegment(p, segment, update_segment);
  }

  [[nodiscard]] DatagramWithSegment<native_methods::GainPtr> with_segment(const native_methods::Segment segment, const bool update_segment) {
    return DatagramWithSegment(std::make_unique<G>(std::move(*this)), segment, update_segment);
  }
};

template <class G>
concept gain = std::derived_from<std::remove_reference_t<G>, IGain>;

class AUTDDatagramChangeGainSegment final {
 public:
  explicit AUTDDatagramChangeGainSegment(const native_methods::Segment segment) : _segment(segment){};

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::AUTDDatagramChangeGainSegment(_segment); }

 private:
  native_methods::Segment _segment;
};

}  // namespace autd3::driver
