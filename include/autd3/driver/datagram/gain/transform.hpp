

#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

#include "autd3/driver/common/drive.hpp"
#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/gain/base.hpp"
#include "autd3/driver/datagram/gain/cache.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain {

template <class F>
concept gain_transform_f = requires(F f, const driver::geometry::Device& dev, const driver::geometry::Transducer& tr, const driver::Drive d) {
  { f(dev, tr, d) } -> std::same_as<driver::Drive>;
};

template <class G, gain_transform_f F>
class Transform final : public driver::GainBase,
                        public driver::IntoDatagramWithSegment<native_methods::GainPtr, Transform<G, F>>,
                        public driver::IntoCache<Transform<G, F>> {
 public:
  Transform(G g, F f) : _g(std::move(g)), _f(std::move(f)) {}
  Transform() = delete;
  Transform(const Transform& obj) = default;
  Transform& operator=(const Transform& obj) = default;
  Transform(Transform&& obj) = default;
  Transform& operator=(Transform&& obj) = default;
  ~Transform() override = default;  // LCOV_EXCL_LINE

  AUTD3_API [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry& geometry) const override {
    std::unordered_map<size_t, std::vector<driver::Drive>> drives;

    const auto res = validate(native_methods::AUTDGainCalc(_g.gain_ptr(geometry), geometry.ptr()));
    std::for_each(geometry.devices().begin(), geometry.devices().end(), [this, &res, &drives](const driver::geometry::Device& dev) {
      std::vector<driver::Drive> d;
      d.resize(dev.num_transducers(), driver::Drive{driver::Phase(0), driver::EmitIntensity::minimum()});
      native_methods::AUTDGainCalcGetResult(res, reinterpret_cast<native_methods::Drive*>(d.data()), static_cast<uint32_t>(dev.idx()));
      std::for_each(dev.cbegin(), dev.cend(), [this, &d, &dev](const driver::geometry::Transducer& tr) { d[tr.idx()] = _f(dev, tr, d[tr.idx()]); });
      drives.emplace(dev.idx(), std::move(d));
    });

    native_methods::AUTDGainCalcFreeResult(res);
    return std::accumulate(geometry.devices().begin(), geometry.devices().end(), native_methods::AUTDGainCustom(),
                           [this, &drives](const native_methods::GainPtr acc, const driver::geometry::Device& dev) {
                             return AUTDGainCustomSet(acc, static_cast<uint32_t>(dev.idx()),
                                                      reinterpret_cast<native_methods::Drive*>(drives[dev.idx()].data()),
                                                      static_cast<uint32_t>(drives[dev.idx()].size()));
                           });
  }

 private:
  G _g;
  F _f;
};
}  // namespace autd3::gain

namespace autd3::driver {

template <class G>
class IntoTransform {
 public:
  IntoTransform() = default;
  IntoTransform(const IntoTransform& obj) = default;
  IntoTransform& operator=(const IntoTransform& obj) = default;
  IntoTransform(IntoTransform&& obj) = default;
  IntoTransform& operator=(IntoTransform&& obj) = default;
  virtual ~IntoTransform() = default;  // LCOV_EXCL_LINE

  template <gain::gain_transform_f F>
  AUTD3_API [[nodiscard]] gain::Transform<G, F> with_transform(F f) & {
    return gain::Transform(*static_cast<G*>(this), std::move(f));
  }
  template <gain::gain_transform_f F>
  AUTD3_API [[nodiscard]] gain::Transform<G, F> with_transform(F f) && {
    return gain::Transform(std::move(*static_cast<G*>(this)), std::move(f));
  }
};

}  // namespace autd3::driver
