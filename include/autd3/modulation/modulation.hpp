#pragma once

#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/datagram/modulation/modulation.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::modulation {

/**
 * @brief Base class for custom modulation
 */
template <class M>
class Modulation : public driver::Modulation<M> {
 public:
  using driver::Modulation<M>::Modulation;

  [[nodiscard]] virtual std::vector<driver::EmitIntensity> calc() const = 0;

  [[nodiscard]] native_methods::ModulationPtr modulation_ptr() const override {
    const auto buffer = calc();
    const auto size = buffer.size();
    return AUTDModulationCustom(static_cast<native_methods::SamplingConfiguration>(this->_config), reinterpret_cast<const uint8_t*>(buffer.data()),
                                size, static_cast<native_methods::LoopBehavior>(this->_loop_behavior));
  }
};

}  // namespace autd3::modulation
