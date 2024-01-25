#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/modulation.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

/**
 * @brief Base class for custom modulation
 */
class Modulation : public driver::Modulation {
 public:
  explicit Modulation(const driver::SamplingConfiguration config) : _config(config) {}

  [[nodiscard]] virtual std::vector<driver::EmitIntensity> calc() const = 0;

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    const auto buffer = calc();
    const auto size = buffer.size();
    return AUTDModulationCustom(static_cast<native_methods::SamplingConfiguration>(_config), reinterpret_cast<const uint8_t*>(buffer.data()), size);
  }

 private:
  driver::SamplingConfiguration _config;
};

}  // namespace autd3::modulation
