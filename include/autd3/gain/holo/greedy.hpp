#pragma once

#include <vector>

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/cache.hpp"
#include "autd3/gain/holo/constraint.hpp"
#include "autd3/gain/holo/holo.hpp"
#include "autd3/gain/transform.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain::holo {

/**
 * @brief Gain to produce multiple foci with greedy algorithm
 *
 * @details Shun Suzuki, Masahiro Fujiwara, Yasutoshi Makino, and Hiroyuki Shinoda, “Radiation Pressure Field Reconstruction for Ultrasound Midair
 * Haptics by Greedy Algorithm with Brute-Force Search,” in IEEE Transactions on Haptics, doi: 10.1109/TOH.2021.3076489
 */
class Greedy final : public Holo<Greedy>, public IntoCache<Greedy>, public IntoTransform<Greedy> {
 public:
  Greedy() : Holo(EmissionConstraint::uniform(driver::EmitIntensity::maximum())), _phase_div(16) {}

  AUTD3_DEF_PARAM(Greedy, uint8_t, phase_div)

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return AUTDGainHoloGreedy(reinterpret_cast<const double*>(this->_foci.data()), reinterpret_cast<const double*>(this->_amps.data()),
                              this->_amps.size(), _phase_div, _constraint.ptr());
  }
};

}  // namespace autd3::gain::holo
