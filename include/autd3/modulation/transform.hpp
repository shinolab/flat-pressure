#pragma once

#include "autd3/driver/datagram/modulation.hpp"
#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

template <class F>
concept modulation_transform_f = requires(F f, size_t idx, driver::EmitIntensity d) {
  { f(idx, d) } -> std::same_as<driver::EmitIntensity>;
};

/**
 * @brief Modulation to transform the result of calculation
 */
template <class M, modulation_transform_f F>
class Transform final : public driver::Modulation, public IntoCache<Transform<M, F>>, public IntoRadiationPressure<Transform<M, F>> {
  using transform_f = uint8_t (*)(const void*, uint32_t, uint8_t);

 public:
  Transform(M m, F f) : _m(std::move(m)), _f(std::move(f)) {
    _f_native = +[](const void* context, const uint32_t i, const uint8_t d) -> uint8_t {
      return static_cast<const Transform*>(context)->_f(static_cast<size_t>(i), driver::EmitIntensity(d)).value();
    };
  }

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return native_methods::AUTDModulationWithTransform(_m.modulation_ptr(), const_cast<void*>(reinterpret_cast<const void*>(_f_native)),
                                                       const_cast<void*>(static_cast<const void*>(this)));
  }

 private:
  M _m;
  F _f;
  transform_f _f_native;
};

template <class M>
class IntoTransform {
 public:
  template <modulation_transform_f F>
  [[nodiscard]] Transform<M, F> with_transform(F f) & {
    return Transform(*static_cast<M*>(this), std::move(f));
  }
  template <modulation_transform_f F>
  [[nodiscard]] Transform<M, F> with_transform(F f) && {
    return Transform(std::move(*static_cast<M*>(this)), std::move(f));
  }
};

}  // namespace autd3::modulation
