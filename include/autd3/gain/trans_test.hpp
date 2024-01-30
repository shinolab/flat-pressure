

#pragma once

#include <optional>

#include "autd3/driver/datagram/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/cache.hpp"
#include "autd3/gain/transform.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain {

template <class F>
concept transducer_test_f = requires(F f, const driver::geometry::Device& d, const driver::geometry::Transducer& tr) {
  { f(d, tr) } -> std::same_as<std::optional<driver::Drive>>;
};

/**
 * @brief Gain to test
 */
template <transducer_test_f F>
class TransducerTest final : public driver::Gain, public IntoCache<TransducerTest<F>>, public IntoTransform<TransducerTest<F>> {
  using native_f = void (*)(const void*, native_methods::GeometryPtr, uint32_t, uint8_t, native_methods::Drive*);

 public:
  explicit TransducerTest(F f) : _f(std::move(f)) {
    _f_native = +[](const void* context, const native_methods::GeometryPtr geometry_ptr, const uint32_t dev_idx, const uint8_t tr_idx,
                    native_methods::Drive* raw) {
      const driver::geometry::Device dev(dev_idx, AUTDDevice(geometry_ptr, dev_idx));
      const driver::geometry::Transducer tr(tr_idx, dev.ptr());
      if (const auto d = static_cast<const TransducerTest*>(context)->_f(dev, tr); d.has_value()) {
        raw->phase = d.value().phase.value();
        raw->intensity = d.value().intensity.value();
      }
    };
  }

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry& geometry) const override {
    return AUTDGainTransducerTest(const_cast<void*>(reinterpret_cast<const void*>(_f_native)),
                                  native_methods::ContextPtr{const_cast<void*>(static_cast<const void*>(this))}, geometry.ptr());
  }

 private:
  F _f;
  native_f _f_native;
};

}  // namespace autd3::gain
