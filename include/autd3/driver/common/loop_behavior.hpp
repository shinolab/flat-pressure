#pragma once

#include "autd3/exception.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::driver {
class LoopBehavior final {
 public:
  [[nodiscard]] static LoopBehavior infinite() noexcept { return LoopBehavior(native_methods::AUTDLoopBehaviorInfinite()); }

  [[nodiscard]] static LoopBehavior finite(const uint32_t v) {
    if (v == 0) throw AUTDException("Value must be greater than 0");
    return LoopBehavior(native_methods::AUTDLoopBehaviorFinite(v));
  }

  [[nodiscard]] static LoopBehavior once() noexcept { return LoopBehavior(native_methods::AUTDLoopBehaviorFinite(1)); }

  explicit operator native_methods::LoopBehavior() const { return _internal; }

 private:
  explicit LoopBehavior(const native_methods::LoopBehavior internal_) : _internal(internal_) {}

  native_methods::LoopBehavior _internal;
};

}  // namespace autd3::driver
