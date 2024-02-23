#pragma once

#include <memory>

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/gain/holo/backend.hpp"
#include "autd3/gain/holo/holo.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain::holo {

/**
 * @brief Gain to produce multiple foci by solving Semi-Definite Programming
 *
 * @details Inoue, Seki, Yasutoshi Makino, and Hiroyuki Shinoda. "Active touch perception produced by airborne ultrasonic haptic hologram." 2015 IEEE
 * World Haptics Conference (WHC). IEEE, 2015.
 */
template <backend B>
class SDP final : public Holo<SDP<B>> {
 public:
  explicit SDP(std::shared_ptr<B> holo_backend)
      : Holo<SDP>(EmissionConstraint::dont_care()), _alpha(1e-3), _repeat(100), _lambda(0.9), _backend(std::move(holo_backend)) {}

  AUTD3_DEF_PARAM(SDP, double, alpha)
  AUTD3_DEF_PARAM(SDP, uint32_t, repeat)
  AUTD3_DEF_PARAM(SDP, double, lambda)

  AUTD3_API [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry&) const override {
    return this->_backend->sdp(reinterpret_cast<const double*>(this->_foci.data()), reinterpret_cast<const double*>(this->_amps.data()),
                               this->_amps.size(), _alpha, _lambda, _repeat, this->_constraint);
  }

 private:
  std::shared_ptr<B> _backend;
};

}  // namespace autd3::gain::holo
