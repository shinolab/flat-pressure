#pragma once

#include "autd3/gain/holo/backend.hpp"
#include "autd3/gain/holo/constraint.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain::holo {

/**
 * @brief Backend using [Nalgebra](https://nalgebra.org/)
 */
class NalgebraBackend final : public Backend {
 public:
  NalgebraBackend() : Backend(native_methods::AUTDNalgebraBackend()) {}
  ~NalgebraBackend() override {
    if (_ptr._0 != nullptr) {
      AUTDDeleteNalgebraBackend(_ptr);
      _ptr._0 = nullptr;
    }
  }
  NalgebraBackend(const NalgebraBackend& v) noexcept = delete;
  NalgebraBackend& operator=(const NalgebraBackend& obj) = delete;
  NalgebraBackend(NalgebraBackend&& obj) = default;
  NalgebraBackend& operator=(NalgebraBackend&& obj) = default;

  AUTD3_API [[nodiscard]] native_methods::GainPtr sdp(const double* foci, const double* amps, const uint64_t size, const double alpha,
                                                      const double lambda, const uint32_t repeat,
                                                      const EmissionConstraint constraint) const override {
    return AUTDGainHoloSDP(_ptr, foci, amps, size, alpha, lambda, repeat, constraint.ptr());
  }
  AUTD3_API [[nodiscard]] native_methods::GainPtr gs(const double* foci, const double* amps, const uint64_t size, const uint32_t repeat,
                                                     const EmissionConstraint constraint) const override {
    return AUTDGainHoloGS(_ptr, foci, amps, size, repeat, constraint.ptr());
  }
  AUTD3_API [[nodiscard]] native_methods::GainPtr gspat(const double* foci, const double* amps, const uint64_t size, const uint32_t repeat,
                                                        const EmissionConstraint constraint) const override {
    return AUTDGainHoloGSPAT(_ptr, foci, amps, size, repeat, constraint.ptr());
  }
  AUTD3_API [[nodiscard]] native_methods::GainPtr naive(const double* foci, const double* amps, const uint64_t size,
                                                        const EmissionConstraint constraint) const override {
    return AUTDGainHoloNaive(_ptr, foci, amps, size, constraint.ptr());
  }
  AUTD3_API [[nodiscard]] native_methods::GainPtr lm(const double* foci, const double* amps, const uint64_t size, const double eps1,
                                                     const double eps2, const double tau, const uint32_t k_max, const double* initial,
                                                     const uint64_t initial_size, const EmissionConstraint constraint) const override {
    return AUTDGainHoloLM(_ptr, foci, amps, size, eps1, eps2, tau, k_max, initial, initial_size, constraint.ptr());
  }
};
}  // namespace autd3::gain::holo
