#pragma once

#include "autd3/gain/holo/backend.hpp"
#include "autd3/gain/holo/constraint.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/autd3capi-backend-cuda.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain::holo {

/**
 * @brief Backend using CUDA
 */
class CUDABackend final : public Backend {
 public:
  CUDABackend() { _ptr = validate(native_methods::AUTDCUDABackend()); }
  ~CUDABackend() override {
    if (_ptr._0 != nullptr) {
      AUTDCUDABackendDelete(_ptr);
      _ptr._0 = nullptr;
    }
  }
  CUDABackend(const CUDABackend& v) noexcept = delete;
  CUDABackend& operator=(const CUDABackend& obj) = delete;
  CUDABackend(CUDABackend&& obj) = default;
  CUDABackend& operator=(CUDABackend&& obj) = default;

  [[nodiscard]] native_methods::GainPtr sdp(const double* foci, const double* amps, const uint64_t size, const double alpha, const double lambda,
                                            const uint32_t repeat, const EmissionConstraint constraint) const override {
    return AUTDGainHoloCUDASDP(_ptr, foci, amps, size, alpha, lambda, repeat, constraint.ptr());
  }
  [[nodiscard]] native_methods::GainPtr gs(const double* foci, const double* amps, const uint64_t size, const uint32_t repeat,
                                           const EmissionConstraint constraint) const override {
    return AUTDGainHoloCUDAGS(_ptr, foci, amps, size, repeat, constraint.ptr());
  }
  [[nodiscard]] native_methods::GainPtr gspat(const double* foci, const double* amps, const uint64_t size, const uint32_t repeat,
                                              const EmissionConstraint constraint) const override {
    return AUTDGainHoloCUDAGSPAT(_ptr, foci, amps, size, repeat, constraint.ptr());
  }
  [[nodiscard]] native_methods::GainPtr naive(const double* foci, const double* amps, const uint64_t size,
                                              const EmissionConstraint constraint) const override {
    return AUTDGainHoloCUDANaive(_ptr, foci, amps, size, constraint.ptr());
  }
  [[nodiscard]] native_methods::GainPtr lm(const double* foci, const double* amps, const uint64_t size, const double eps1, const double eps2,
                                           const double tau, const uint32_t k_max, const double* initial, const uint64_t initial_size,
                                           const EmissionConstraint constraint) const override {
    return AUTDGainHoloCUDALM(_ptr, foci, amps, size, eps1, eps2, tau, k_max, constraint.ptr(), initial, initial_size);
  }
};

}  // namespace autd3::gain::holo
