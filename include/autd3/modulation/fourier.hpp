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
class Fourier final : public driver::Modulation<Fourier>,
                      public IntoCache<Fourier>,
                      public IntoTransform<Fourier>,
                      public IntoRadiationPressure<Fourier> {
 public:
  explicit Fourier(Sine component) { _components.emplace_back(std::move(component)); }

  AUTD3_API void add_component(Sine component) & { _components.emplace_back(std::move(component)); }

  AUTD3_API [[nodiscard]] Fourier&& add_component(Sine component) && {
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
  AUTD3_API void add_components_from_iter(R&& iter) & {
    for (Sine e : iter) _components.emplace_back(std::move(e));
  }

  /**
   * @brief Add components from iterator
   *
   * @tparam R
   * @param iter iterator of focus points
   */
  template <fourier_sine_range R>
  AUTD3_API [[nodiscard]] Fourier add_components_from_iter(R&& iter) && {
    for (Sine e : iter) _components.emplace_back(std::move(e));
    return std::move(*this);
  }

  AUTD3_API [[nodiscard]] Fourier& operator+=(const Sine& rhs) {
    _components.emplace_back(rhs);
    return *this;
  }

  AUTD3_API [[nodiscard]] friend Fourier&& operator+(Fourier&& lhs, const Sine& rhs) {
    lhs._components.emplace_back(rhs);
    return std::move(lhs);
  }

  AUTD3_API [[nodiscard]] friend Fourier operator+(Sine&& lhs, const Sine& rhs) {
    Fourier m(lhs);
    m._components.emplace_back(rhs);
    return m;
  }  // LCOV_EXCL_LINE

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    std::vector<native_methods::ModulationPtr> components;
    components.reserve(_components.size());
    std::ranges::transform(_components, std::back_inserter(components), [&](const auto& m) { return m.modulation_ptr(); });
    return AUTDModulationFourier(components.data(), static_cast<uint32_t>(components.size()),
                                 static_cast<native_methods::LoopBehavior>(_loop_behavior));
  }

 private:
  std::vector<Sine> _components;
};

}  // namespace autd3::modulation
