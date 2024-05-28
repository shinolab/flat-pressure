#pragma once

#include "autd3/gain/holo/constraint.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain::holo {

/**
 * @brief Calculation backend
 */
class Backend {
 public:
  Backend() noexcept : _ptr(native_methods::BackendPtr{nullptr}) {}
  explicit Backend(const native_methods::BackendPtr ptr) noexcept : _ptr(ptr) {}
  Backend(const Backend& obj) = default;
  Backend& operator=(const Backend& obj) = default;
  Backend(Backend&& obj) = default;
  Backend& operator=(Backend&& obj) = default;
  virtual ~Backend() = default;  // LCOV_EXCL_LINE

  [[nodiscard]] native_methods::BackendPtr ptr() const { return _ptr; }

  [[nodiscard]] virtual native_methods::GainPtr sdp(const double* foci, const double* amps, uint64_t size, double alpha, double lambda,
                                                    uint32_t repeat, EmissionConstraint constraint) const = 0;
  [[nodiscard]] virtual native_methods::GainPtr gs(const double* foci, const double* amps, uint64_t size, uint32_t repeat,
                                                   EmissionConstraint constraint) const = 0;
  [[nodiscard]] virtual native_methods::GainPtr gspat(const double* foci, const double* amps, uint64_t size, uint32_t repeat,
                                                      EmissionConstraint constraint) const = 0;
  [[nodiscard]] virtual native_methods::GainPtr naive(const double* foci, const double* amps, uint64_t size, EmissionConstraint constraint) const = 0;
  [[nodiscard]] virtual native_methods::GainPtr lm(const double* foci, const double* amps, uint64_t size, double eps1, double eps2, double tau,
                                                   uint32_t k_max, const double* initial, uint64_t initial_size,
                                                   EmissionConstraint constraint) const = 0;

 protected:
  native_methods::BackendPtr _ptr;
};

template <class B>
concept backend = requires(B b) {
  { b.ptr() } -> std::same_as<native_methods::BackendPtr>;
};

}  // namespace autd3::gain::holo
