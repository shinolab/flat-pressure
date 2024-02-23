#pragma once

#include <memory>

#include "autd3/driver/datagram/datagram.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <typename P>
class DatagramS {
 public:
  virtual ~DatagramS() = default;
  AUTD3_API virtual P raw_ptr(const geometry::Geometry&) const = 0;
  AUTD3_API virtual native_methods::DatagramPtr into_segment(const P p, const native_methods::Segment segment, const bool update_segment) const = 0;
};

template <typename P>
class DatagramWithSegment {
 public:
  explicit DatagramWithSegment(std::unique_ptr<DatagramS<P>> datagram, const native_methods::Segment segment, const bool update_segment)
      : _datagram(std::move(datagram)), _segment(segment), _update_segment(update_segment) {}
  ~DatagramWithSegment() = default;
  DatagramWithSegment(const DatagramWithSegment& v) noexcept = default;
  DatagramWithSegment& operator=(const DatagramWithSegment& obj) = default;
  DatagramWithSegment(DatagramWithSegment&& obj) = default;
  DatagramWithSegment& operator=(DatagramWithSegment&& obj) = default;

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& g) {
    auto raw_ptr = _datagram->raw_ptr(g);
    return _datagram->into_segment(raw_ptr, _segment, _update_segment);
  }

 private:
  std::unique_ptr<DatagramS<P>> _datagram;
  native_methods::Segment _segment;
  bool _update_segment;
};

template <typename P, class D>
class IntoDatagramWithSegment {
 public:
  virtual ~IntoDatagramWithSegment() = default;  // LCOV_EXCL_LINE

  AUTD3_API [[nodiscard]] DatagramWithSegment<P> with_segment(const native_methods::Segment segment, const bool update_segment) & {
    return DatagramWithSegment<P>(std::make_unique<D>(*static_cast<D*>(this)), segment, update_segment);
  }
  AUTD3_API [[nodiscard]] DatagramWithSegment<P> with_segment(const native_methods::Segment segment, const bool update_segment) && {
    return DatagramWithSegment<P>(std::make_unique<D>((std::move(*static_cast<D*>(this)))), segment, update_segment);
  }
};

}  // namespace autd3::driver
