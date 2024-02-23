#pragma once

#include <memory>

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/modulation/base.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

/**
 * @brief Modulation to cache the result of calculation
 */
template <class M>
class Cache final : public driver::ModulationBase<Cache<M>> {
 public:
  explicit Cache(M m) : _m(std::move(m)), _cache(std::make_shared<std::vector<driver::EmitIntensity>>()) {}
  Cache(const Cache& v) = default;
  Cache& operator=(const Cache& obj) = delete;
  Cache(Cache&& obj) noexcept = default;
  Cache& operator=(Cache&& obj) noexcept = delete;
  ~Cache() noexcept override = default;  // LCOV_EXCL_LINE

  AUTD3_API const std::vector<driver::EmitIntensity>& calc() const { return init(); }

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    const auto buf = calc();
    return AUTDModulationCustom(static_cast<native_methods::SamplingConfiguration>(_sampling_config.value()),
                                reinterpret_cast<const uint8_t*>(buf.data()), static_cast<uint64_t>(buf.size()),
                                static_cast<native_methods::LoopBehavior>(_m.loop_behavior()));
  }

  AUTD3_API [[nodiscard]] const std::vector<driver::EmitIntensity>& buffer() const { return *_cache; }

  AUTD3_API [[nodiscard]] std::vector<driver::EmitIntensity>::const_iterator cbegin() const noexcept { return _cache->cbegin(); }
  AUTD3_API [[nodiscard]] std::vector<driver::EmitIntensity>::const_iterator cend() const noexcept { return _cache->cend(); }
  AUTD3_API [[nodiscard]] std::vector<driver::EmitIntensity>::const_iterator begin() const noexcept { return _cache->begin(); }
  AUTD3_API [[nodiscard]] std::vector<driver::EmitIntensity>::const_iterator end() const noexcept { return _cache->end(); }

  AUTD3_API [[nodiscard]] const driver::EmitIntensity& operator[](const size_t i) const { return _cache->at(i); }

 private:
  const std::vector<driver::EmitIntensity>& init() const {
    if (_cache->size() == 0) {
      const auto res = native_methods::AUTDModulationCalc(_m.modulation_ptr());
      const auto ptr = validate(res);
      _cache->resize(res.result_len, driver::EmitIntensity(0));
      _sampling_config = driver::SamplingConfiguration::from_frequency_division(res.freq_div);
      native_methods::AUTDModulationCalcGetResult(ptr, reinterpret_cast<uint8_t*>(_cache->data()));
    }
    return *_cache;
  }

  M _m;
  mutable std::shared_ptr<std::vector<driver::EmitIntensity>> _cache;
  mutable std::optional<driver::SamplingConfiguration> _sampling_config;
};

}  // namespace autd3::modulation

namespace autd3::driver {

template <class M>
class IntoModulationCache {
 public:
  AUTD3_API [[nodiscard]] modulation::Cache<M> with_cache() & { return modulation::Cache(*static_cast<M*>(this)); }
  AUTD3_API [[nodiscard]] modulation::Cache<M> with_cache() && { return modulation::Cache(std::move(*static_cast<M*>(this))); }
};

}  // namespace autd3::driver
