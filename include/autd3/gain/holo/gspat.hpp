#pragma once

#include <memory>

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/holo/holo.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain::holo {

/**
 * @brief Gain to produce multiple foci with GS-PAT algorithm
 *
 * @details Diego Martinez Plasencia et al. "Gs-pat: high-speed multi-point sound-fields for phased arrays of transducers," ACMTrans-actions on
 * Graphics (TOG), 39(4):138–1, 2020.
 */
template <backend B>
class GSPAT final : public Holo<GSPAT<B>> {
 public:
  explicit GSPAT(std::shared_ptr<B> holo_backend) : Holo<GSPAT>(EmissionConstraint::dont_care()), _repeat(100), _backend(std::move(holo_backend)) {}

  AUTD3_DEF_PARAM(GSPAT, uint32_t, repeat)

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return this->_backend->gspat(reinterpret_cast<const double*>(this->_foci.data()), reinterpret_cast<const double*>(this->_amps.data()),
                                 this->_amps.size(), _repeat, this->_constraint);
  }

 private:
  std::shared_ptr<B> _backend;
};

}  // namespace autd3::gain::holo
