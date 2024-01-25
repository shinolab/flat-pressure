#pragma once

#include <numeric>

#include "autd3/modulation/cache.hpp"
#include "autd3/modulation/radiation_pressure.hpp"
#include "autd3/modulation/sine.hpp"
#include "autd3/modulation/transform.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

template <class R>
concept fourier_sine_range = std::ranges::viewable_range<R> && std::same_as<std::ranges::range_value_t<R>, Sine>;

/**
 * @brief Multi-frequency sine wave modulation
 */
class Fourier final : public driver::Modulation, public IntoCache<Fourier>, public IntoTransform<Fourier>, public IntoRadiationPressure<Fourier> {
 public:
  explicit Fourier(Sine component) { _components.emplace_back(std::move(component)); }

  void add_component(Sine component) & { _components.emplace_back(std::move(component)); }

  [[nodiscard]] Fourier&& add_component(Sine component) && {
    _components.emplace_back(std::move(component));
    return std::move(*this);
  }

  /**
   * @brief Add components from iterator
   *
   * @tparam R
   * @param iter iterator of focus points
   */
  template <fourier_sine_range R>
  void add_components_from_iter(R&& iter) & {
    for (Sine e : iter) _components.emplace_back(std::move(e));
  }

  /**
   * @brief Add components from iterator
   *
   * @tparam R
   * @param iter iterator of focus points
   */
  template <fourier_sine_range R>
  Fourier add_components_from_iter(R&& iter) && {
    for (Sine e : iter) _components.emplace_back(std::move(e));
    return std::move(*this);
  }

  Fourier& operator+=(const Sine& rhs) {
    _components.emplace_back(rhs);
    return *this;
  }

  friend Fourier&& operator+(Fourier&& lhs, const Sine& rhs) {
    lhs._components.emplace_back(rhs);
    return std::move(lhs);
  }

  friend Fourier operator+(Sine&& lhs, const Sine& rhs) {
    Fourier m(lhs);
    m._components.emplace_back(rhs);
    return m;
  }  // LCOV_EXCL_LINE

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    return std::accumulate(
        _components.begin() + 1, _components.end(), AUTDModulationFourier(_components[0].modulation_ptr()),
        [](const native_methods::ModulationPtr ptr, const Sine& sine) { return AUTDModulationFourierAddComponent(ptr, sine.modulation_ptr()); });
  }

 private:
  std::vector<Sine> _components;
};

}  // namespace autd3::modulation
