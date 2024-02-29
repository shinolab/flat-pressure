#pragma once

#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/driver/datagram/modulation/base.hpp"
#include "autd3/driver/datagram/modulation/cache.hpp"
#include "autd3/driver/datagram/modulation/radiation_pressure.hpp"
#include "autd3/driver/datagram/modulation/transform.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::driver {

template <class M>
class Modulation : public ModulationBase<M>, public IntoModulationCache<M>, public IntoRadiationPressure<M>, public IntoModulationTransform<M> {
 protected:
  SamplingConfiguration _config;

  explicit Modulation(const SamplingConfiguration config) : _config(config) {}

 public:
  void with_sampling_config(const SamplingConfiguration config) & { _config = config; }  // LCOV_EXCL_LINE
  [[nodiscard]] M&& with_sampling_config(const SamplingConfiguration config) && {
    _config = config;
    return std::move(*static_cast<M*>(this));
  }
};

}  // namespace autd3::driver
