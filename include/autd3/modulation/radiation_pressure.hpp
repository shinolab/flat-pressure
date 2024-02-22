#pragma once

#include "autd3/driver/datagram/modulation.hpp"
#include "autd3/modulation/cache.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

/**
 * @brief Modulation for modulating radiation pressure
 */
template <class M>
class RadiationPressure final : public driver::Modulation<RadiationPressure<M>>, public IntoCache<RadiationPressure<M>> {
 public:
  explicit RadiationPressure(M m) : _m(std::move(m)) { this->_loop_behavior = _m.loop_behavior(); }

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return native_methods::AUTDModulationWithRadiationPressure(_m.modulation_ptr(), static_cast<native_methods::LoopBehavior>(this->_loop_behavior));
  }

 private:
  M _m;
};

template <class M>
class IntoRadiationPressure {
 public:
  AUTD3_API [[nodiscard]] RadiationPressure<M> with_radiation_pressure() & { return RadiationPressure(*static_cast<M*>(this)); }
  AUTD3_API [[nodiscard]] RadiationPressure<M> with_radiation_pressure() && { return RadiationPressure(std::move(*static_cast<M*>(this))); }
};

}  // namespace autd3::modulation
