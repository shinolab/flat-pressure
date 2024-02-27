#pragma once

#include <optional>

#include "autd3/controller/controller.hpp"
#include "autd3/driver/autd3_device.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/driver/link.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::controller {

/**
 * @brief Builder for Controller
 */
class ControllerBuilder {
 public:
  /**
   * @brief Add device
   *
   * @param device AUTD3 device
   * @return Builder
   */
  AUTD3_API ControllerBuilder add_device(const driver::AUTD3& device) {
    const auto pos = device.position();
    const auto rot = device.rotation();
    _ptr = AUTDControllerBuilderAddDevice(_ptr, pos.x(), pos.y(), pos.z(), rot.w(), rot.x(), rot.y(), rot.z());
    return *this;
  }

  /**
   * @brief Open controller
   *
   * @tparam B LinkBuilder
   * @param link_builder link builder
   * @param timeout timeout
   * @return Controller
   */
  template <driver::link_builder B, typename Rep, typename Period>
  AUTD3_API [[nodiscard]] Controller<typename B::Link> open_with_timeout(B&& link_builder, const std::chrono::duration<Rep, Period> timeout) {
    const int64_t timeout_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout.value()).count();
    auto ptr = validate(AUTDControllerOpen(_ptr, link_builder.ptr(), timeout_ns));
    driver::geometry::Geometry geometry(AUTDGeometry(ptr));
    return Controller<typename B::Link>{std::move(geometry), ptr, link_builder.resolve_link(native_methods::AUTDLinkGet(ptr))};
  }

  /**
   * @brief Open controller
   *
   * @tparam B LinkBuilder
   * @param link_builder link builder
   * @return Controller
   */
  template <driver::link_builder B>
  AUTD3_API [[nodiscard]] Controller<typename B::Link> open(B&& link_builder) {
    auto ptr = validate(AUTDControllerOpen(_ptr, link_builder.ptr(), -1));
    driver::geometry::Geometry geometry(AUTDGeometry(ptr));
    return Controller<typename B::Link>{std::move(geometry), ptr, link_builder.resolve_link(native_methods::AUTDLinkGet(ptr))};
  }

#ifdef AUTD3_ASYNC_API
  /**
   * @brief Open controller
   *
   * @tparam B LinkBuilder
   * @param link_builder link builder
   * @return Controller
   */
  template <driver::link_builder B, typename Rep, typename Period>
  AUTD3_API [[nodiscard]] coro::task<Controller<typename B::Link>> open(B&& link_builder, const std::chrono::duration<Rep, Period> timeout) {
    co_return open_with_timeout(std::forward<B>(link_builder), timeout);
  }

  /**
   * @brief Open controller
   *
   * @tparam B LinkBuilder
   * @param link_builder link builder
   * @return Controller
   */
  template <driver::link_builder B>
  AUTD3_API [[nodiscard]] coro::task<Controller<typename B::Link>> open_async(B&& link_builder) {
    co_return open(std::forward<B>(link_builder));
  }
#endif

  AUTD3_API ControllerBuilder() : _ptr(native_methods::AUTDControllerBuilder()) {}

 private:
  native_methods::ControllerBuilderPtr _ptr;
};

}  // namespace autd3::controller
