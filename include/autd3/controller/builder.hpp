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
  ControllerBuilder add_device(const driver::AUTD3& device) {
    const auto rot = device.rotation().has_value() ? device.rotation().value() : driver::Quaternion::Identity();
    _ptr =
        AUTDControllerBuilderAddDevice(_ptr, device.position().x(), device.position().y(), device.position().z(), rot.w(), rot.x(), rot.y(), rot.z());
    return *this;
  }

  /**
   * @brief Open controller
   *
   * @tparam B LinkBuilder
   * @param link_builder link builder
   * @return Controller
   */
  template <driver::link_builder B>
  [[nodiscard]] Controller<typename B::Link> open_with(B&& link_builder) {
    auto ptr = validate(AUTDControllerOpenWith(_ptr, link_builder.ptr()));
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
  template <driver::link_builder B>
  [[nodiscard]] coro::task<Controller<typename B::Link>> open_with_async(B&& link_builder) {
      co_return open_with(std::forward<B>(link_builder));
  }
#endif

  ControllerBuilder() : _ptr(native_methods::AUTDControllerBuilder()) {}

 private:
  native_methods::ControllerBuilderPtr _ptr;
};

}  // namespace autd3::controller
