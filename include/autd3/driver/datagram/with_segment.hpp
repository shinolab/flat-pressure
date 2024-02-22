#pragma once

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <typename P, class D>
class DatagramWithSegment {
 public:
  explicit DatagramWithSegment(D datagram, const native_methods::Segment segment, const bool update_segment)
      : _datagram(std::move(datagram)), _segment(segment), _update_segment(update_segment) {}
  ~DatagramWithSegment() = default;
  DatagramWithSegment(const DatagramWithSegment& v) noexcept = default;
  DatagramWithSegment& operator=(const DatagramWithSegment& obj) = default;
  DatagramWithSegment(DatagramWithSegment&& obj) = default;
  DatagramWithSegment& operator=(DatagramWithSegment&& obj) = default;

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& g) {
    auto raw_ptr = _datagram.raw_ptr(g);
    return _datagram.into_segment(raw_ptr, _segment, _update_segment);
  }

 private:
  D _datagram;
  native_methods::Segment _segment;
  bool _update_segment;
};

template <typename P, class D>
class IntoDatagramWithSegment {
 public:
  virtual P raw_ptr(const geometry::Geometry&) const = 0;
  virtual native_methods::DatagramPtr into_segment(const P p, const native_methods::Segment segment, const bool update_segment) const = 0;

  [[nodiscard]] DatagramWithSegment<P, D> with_segment(const native_methods::Segment segment, const bool update_segment) {
    return DatagramWithSegment(std::move(this), segment, update_segment);
  }
};

}  // namespace autd3::driver
