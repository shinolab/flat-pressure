#pragma once

#include "autd3/exception.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::link {
class Audit;
}

namespace autd3::driver {

class LoopBehavior final {
 public:
  friend class autd3::link::Audit;

  AUTD3_API [[nodiscard]] static LoopBehavior infinite() noexcept { return LoopBehavior(native_methods::AUTDLoopBehaviorInfinite()); }

  AUTD3_API [[nodiscard]] static LoopBehavior finite(const uint32_t v) {
    if (v == 0) throw AUTDException("Value must be greater than 0");
    return LoopBehavior(native_methods::AUTDLoopBehaviorFinite(v));
  }

  AUTD3_API [[nodiscard]] static LoopBehavior once() noexcept { return LoopBehavior(native_methods::AUTDLoopBehaviorOnce()); }

  AUTD3_API explicit operator native_methods::LoopBehavior() const { return _internal; }
  AUTD3_API auto operator<=>(const LoopBehavior& other) const { return _internal.rep <=> other._internal.rep; }
  AUTD3_API bool operator==(const LoopBehavior& other) const { return _internal.rep == other._internal.rep; }
  // LCOV_EXCL_LINE
 private:
  AUTD3_API explicit LoopBehavior(const native_methods::LoopBehavior internal_) : _internal(internal_) {}

  native_methods::LoopBehavior _internal;
};

}  // namespace autd3::driver
