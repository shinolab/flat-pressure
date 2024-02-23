#pragma once

#include "autd3/driver/datagram/modulation/base.hpp"
#include "autd3/driver/datagram/modulation/cache.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

/**
 * @brief Modulation for modulating radiation pressure
 */
template <class M>
class RadiationPressure final : public driver::ModulationBase<RadiationPressure<M>>, public driver::IntoModulationCache<RadiationPressure<M>> {
 public:
  explicit RadiationPressure(M m) : _m(std::move(m)) { this->_loop_behavior = _m.loop_behavior(); }

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return native_methods::AUTDModulationWithRadiationPressure(_m.modulation_ptr(), static_cast<native_methods::LoopBehavior>(this->_loop_behavior));
  }

 private:
  M _m;
};

}  // namespace autd3::modulation

namespace autd3::driver {

template <class M>
class IntoRadiationPressure {
 public:
  AUTD3_API [[nodiscard]] modulation::RadiationPressure<M> with_radiation_pressure() & {
    return modulation::RadiationPressure(*static_cast<M*>(this));
  }
  AUTD3_API [[nodiscard]] modulation::RadiationPressure<M> with_radiation_pressure() && {
    return modulation::RadiationPressure(std::move(*static_cast<M*>(this)));
  }
};

}  // namespace autd3::driver
