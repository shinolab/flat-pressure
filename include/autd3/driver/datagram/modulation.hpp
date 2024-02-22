#pragma once

#include "autd3/driver/common/loop_behavior.hpp"
#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::driver {

template <class M>
class Modulation : public DatagramS<native_methods::ModulationPtr> {
 public:
  Modulation() : _loop_behavior(LoopBehavior::infinite()) {}
  Modulation(const Modulation& obj) = default;
  Modulation& operator=(const Modulation& obj) = default;
  Modulation(Modulation&& obj) = default;
  Modulation& operator=(Modulation&& obj) = default;
  virtual ~Modulation() = default;  // LCOV_EXCL_LINE

  /**
   * @brief Get sampling frequency division
   * @details The sampling frequency is
   * [autd3::native_methods::FPGA_CLK_FREQ] / (sampling frequency
   * division).
   */
  AUTD3_API [[nodiscard]] SamplingConfiguration sampling_config() const {
    return SamplingConfiguration(AUTDModulationSamplingConfig(modulation_ptr()));
  }

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) const { return AUTDModulationIntoDatagram(modulation_ptr()); }

  AUTD3_API [[nodiscard]] native_methods::ModulationPtr raw_ptr(const geometry::Geometry&) const override { return modulation_ptr(); }

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr into_segment(const native_methods::ModulationPtr p, const native_methods::Segment segment,
                                                                   const bool update_segment) const override {
    return AUTDModulationIntoDatagramWithSegment(p, segment, update_segment);
  }

  AUTD3_API [[nodiscard]] virtual native_methods::ModulationPtr modulation_ptr() const = 0;

  AUTD3_API [[nodiscard]] size_t size() const { return native_methods::validate<size_t>(AUTDModulationSize(modulation_ptr())); }

  AUTD3_API [[nodiscard]] LoopBehavior loop_behavior() const noexcept { return _loop_behavior; }

  void with_loop_behavior(const LoopBehavior loop_behavior) & { _loop_behavior = loop_behavior; }
  AUTD3_API [[nodiscard]] M&& with_loop_behavior(const LoopBehavior loop_behavior) && {
    _loop_behavior = loop_behavior;
    return std::move(*static_cast<M*>(this));
  }

  AUTD3_API [[nodiscard]] DatagramWithSegment<native_methods::ModulationPtr> with_segment(const native_methods::Segment segment,
                                                                                          const bool update_segment) {
    return DatagramWithSegment<native_methods::ModulationPtr>(std::make_unique<M>((std::move(*static_cast<M*>(this)))), segment, update_segment);
  }

 protected:
  LoopBehavior _loop_behavior;
};

template <class M>
class ModulationWithSamplingConfig : public Modulation<M> {
 protected:
  SamplingConfiguration _config;

  explicit ModulationWithSamplingConfig(const SamplingConfiguration config) : _config(config) {}

 public:
  AUTD3_API void with_sampling_config(const SamplingConfiguration config) & { _config = config; }
  AUTD3_API [[nodiscard]] M&& with_sampling_config(const SamplingConfiguration config) && {
    _config = config;
    return std::move(*static_cast<M*>(this));
  }
};

class ChangeModulationSegment final {
 public:
  explicit ChangeModulationSegment(const native_methods::Segment segment) : _segment(segment){};

  AUTD3_API [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) {
    return native_methods::AUTDDatagramChangeModulationSegment(_segment);
  }

 private:
  native_methods::Segment _segment;
};

}  // namespace autd3::driver
