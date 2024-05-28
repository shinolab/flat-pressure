#pragma once

#include <memory>
#include <vector>

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/holo/holo.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain::holo {

/**
 * @brief Gain to produce multiple foci with Levenberg-Marquardt algorithm
 *
 * @details
 * * K.Levenberg, “A method for the solution of certain non-linear problems in least squares,” Quarterly of applied mathematics, vol.2,
 * no.2, pp.164–168, 1944.
 * * D.W.Marquardt, “An algorithm for least-squares estimation of non-linear parameters,” Journal of the society for Industrial and
 * AppliedMathematics, vol.11, no.2, pp.431–441, 1963.
 * * K.Madsen, H.Nielsen, and O.Tingleff, “Methods for non-linear least squares problems (2nd ed.),” 2004.
 */
template <backend B>
class LM final : public Holo<LM<B>> {
 public:
  explicit LM(std::shared_ptr<B> holo_backend)
      : Holo<LM>(EmissionConstraint::dont_care()), _eps1(1e-8), _eps2(1e-8), _tau(1e-3), _k_max(5), _backend(std::move(holo_backend)) {}

  AUTD3_DEF_PARAM(LM, double, eps1)
  AUTD3_DEF_PARAM(LM, double, eps2)
  AUTD3_DEF_PARAM(LM, double, tau)
  AUTD3_DEF_PARAM(LM, uint32_t, k_max)

  void with_initial(std::vector<double> value) & { _initial = std::move(value); }
  [[nodiscard]] LM&& with_initial(std::vector<double> value) && {
    _initial = std::move(value);
    return std::move(*this);
  }

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return this->_backend->lm(reinterpret_cast<const double*>(this->_foci.data()), reinterpret_cast<const double*>(this->_amps.data()),
                              this->_amps.size(), _eps1, _eps2, _tau, _k_max, _initial.data(), _initial.size(), this->_constraint);
  }

 private:
  std::shared_ptr<B> _backend;
  std::vector<double> _initial;
};

}  // namespace autd3::gain::holo
