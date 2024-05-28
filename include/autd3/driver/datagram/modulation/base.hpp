#pragma once

#include "autd3/driver/common/loop_behavior.hpp"
#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::driver {

template <class M>
class ModulationBase : public DatagramS<native_methods::ModulationPtr>, public IntoDatagramWithSegment<native_methods::ModulationPtr, M> {
 public:
  ModulationBase() : _loop_behavior(LoopBehavior::infinite()) {}   // LCOV_EXCL_LINE
  ModulationBase(const ModulationBase& obj) = default;             // LCOV_EXCL_LINE
  ModulationBase& operator=(const ModulationBase& obj) = default;  // LCOV_EXCL_LINE
  ModulationBase(ModulationBase&& obj) = default;                  // LCOV_EXCL_LINE
  ModulationBase& operator=(ModulationBase&& obj) = default;       // LCOV_EXCL_LINE
  virtual ~ModulationBase() = default;                             // LCOV_EXCL_LINE

  /**
   * @brief Get sampling frequency division
   * @details The sampling frequency is
   * [autd3::native_methods::FPGA_CLK_FREQ] / (sampling frequency
   * division).
   */
  [[nodiscard]] SamplingConfiguration sampling_config() const { return SamplingConfiguration(AUTDModulationSamplingConfig(modulation_ptr())); }

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) const { return AUTDModulationIntoDatagram(modulation_ptr()); }

  // LCOV_EXCL_START
  [[nodiscard]] native_methods::ModulationPtr raw_ptr(const geometry::Geometry&) const override { return modulation_ptr(); }
  [[nodiscard]] native_methods::DatagramPtr into_segment(const native_methods::ModulationPtr p, const native_methods::Segment segment,
                                                         const bool update_segment) const override {
    return AUTDModulationIntoDatagramWithSegment(p, segment, update_segment);
  }
  [[nodiscard]] virtual native_methods::ModulationPtr modulation_ptr() const = 0;
  // LCOV_EXCL_STOP

  [[nodiscard]] size_t size() const { return native_methods::validate<size_t>(AUTDModulationSize(modulation_ptr())); }

  [[nodiscard]] LoopBehavior loop_behavior() const noexcept { return _loop_behavior; }

  void with_loop_behavior(const LoopBehavior loop_behavior) & { _loop_behavior = loop_behavior; }  // LCOV_EXCL_LINE
  [[nodiscard]] M&& with_loop_behavior(const LoopBehavior loop_behavior) && {
    _loop_behavior = loop_behavior;
    return std::move(*static_cast<M*>(this));
  }

 protected:
  LoopBehavior _loop_behavior;
};

class ChangeModulationSegment final {
 public:
  explicit ChangeModulationSegment(const native_methods::Segment segment) : _segment(segment){};

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry&) { return native_methods::AUTDDatagramChangeModulationSegment(_segment); }

 private:
  native_methods::Segment _segment;
};

}  // namespace autd3::driver
