#pragma once

#include <chrono>
#include <future>
#include <optional>
#include <vector>

#ifdef AUTD3_ASYNC_API
#include <coro/coro.hpp>
#endif

#include "autd3/driver/datagram/datagram.hpp"
#include "autd3/driver/firmware_version.hpp"
#include "autd3/driver/fpga/defined/fpga_state.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/exception.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::controller {

class ControllerBuilder;

template <class F>
concept group_f =
    requires(F f, const driver::geometry::Device& dev) { typename std::invoke_result_t<F, const driver::geometry::Device&>::value_type; };

/**
 * @brief Controller class for AUTD3
 */
template <typename L>
class Controller {
  friend class ControllerBuilder;

 public:
  Controller() = delete;
  Controller(const Controller& v) = delete;
  Controller& operator=(const Controller& obj) = delete;
  Controller(Controller&& obj) noexcept : _geometry(std::move(obj._geometry)), _ptr(obj._ptr), _link(std::move(obj._link)) { obj._ptr._0 = nullptr; }
  Controller& operator=(Controller&& obj) noexcept {
    if (this != &obj) {
      _geometry = std::move(obj._geometry);
      _ptr = obj._ptr;
      _link = std::move(obj._link);
      obj._ptr._0 = nullptr;
    }
    return *this;
  }

  ~Controller() noexcept {
    if (_ptr._0 != nullptr) {
      AUTDControllerDelete(_ptr);
      _ptr._0 = nullptr;
    }
  }

  AUTD3_API [[nodiscard]] const driver::geometry::Geometry& geometry() const { return _geometry; }
  AUTD3_API [[nodiscard]] driver::geometry::Geometry& geometry() { return _geometry; }

  AUTD3_API [[nodiscard]] L& link() { return _link; }
  AUTD3_API [[nodiscard]] const L& link() const { return _link; }

  /**
   * @brief Close connection
   */
  AUTD3_API bool close() const { return validate(AUTDControllerClose(_ptr)) == native_methods::AUTD3_TRUE; }

  /**
   * @brief Get FPGA information
   *
   * @return List of FPGA information
   */
  AUTD3_API [[nodiscard]] std::vector<std::optional<driver::FPGAState>> fpga_info() {
    const size_t num_devices = geometry().num_devices();
    std::vector<int32_t> info(num_devices);
    validate(AUTDControllerFPGAState(_ptr, info.data()));
    std::vector<std::optional<driver::FPGAState>> ret;
    ret.reserve(num_devices);
    std::ranges::transform(info, std::back_inserter(ret), [](const int32_t i) -> std::optional<driver::FPGAState> {
      return i < 0 ? std::nullopt : std::optional(driver::FPGAState(static_cast<uint8_t>(i)));
    });
    return ret;
  }

  /**
   * @brief Get firmware information
   *
   * @return List of firmware information
   */
  AUTD3_API [[nodiscard]] std::vector<driver::FirmwareInfo> firmware_infos() {
    const auto handle = validate(AUTDControllerFirmwareInfoListPointer(_ptr));
    std::vector<driver::FirmwareInfo> ret;
    for (uint32_t i = 0; i < static_cast<uint32_t>(geometry().num_devices()); i++) {
      char info[256]{};
      AUTDControllerFirmwareInfoGet(handle, i, info);
      ret.emplace_back(std::string(info));
    }
    AUTDControllerFirmwareInfoListPointerDelete(handle);
    return ret;
  }  // LCOV_EXCL_LINE

  /**
   * @brief Send data to the devices
   *
   * @tparam D Datagram
   * @tparam Rep
   * @tparam Period
   * @param data data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D, typename Rep, typename Period>
  AUTD3_API bool send(D&& data, const std::chrono::duration<Rep, Period> timeout) {
    return send(std::forward<D>(data), std::optional(timeout));
  }

  /**
   * @brief Send data to the devices
   *
   * @tparam D Datagram
   * @tparam Rep
   * @tparam Period
   * @param data data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D, typename Rep = uint64_t, typename Period = std::milli>
  AUTD3_API bool send(D&& data, const std::optional<std::chrono::duration<Rep, Period>> timeout = std::nullopt) {
    return send(std::forward<D>(data), driver::NullDatagram(), timeout);
  }

  /**
   * @brief Send data to the devices
   *
   * @tparam D1 Datagram
   * @tparam D2 Datagram
   * @tparam Rep
   * @tparam Period
   * @param data1 first data
   * @param data2 second data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D1, driver::datagram D2, typename Rep, typename Period>
  AUTD3_API bool send(D1&& data1, D2&& data2, const std::chrono::duration<Rep, Period> timeout) {
    return send(std::forward<D1>(data1), std::forward<D2>(data2), std::optional(timeout));
  }

  /**
   * @brief Send data to the devices
   *
   * @tparam D1 Datagram
   * @tparam D2 Datagram
   * @tparam Rep
   * @tparam Period
   * @param data1 first data
   * @param data2 second data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D1, driver::datagram D2, typename Rep = uint64_t, typename Period = std::milli>
  AUTD3_API bool send(D1&& data1, D2&& data2, const std::optional<std::chrono::duration<Rep, Period>> timeout = std::nullopt) {
    const int64_t timeout_ns = timeout.has_value() ? std::chrono::duration_cast<std::chrono::nanoseconds>(timeout.value()).count() : -1;
    return validate(AUTDControllerSend(_ptr, data1.ptr(_geometry), data2.ptr(_geometry), timeout_ns)) == native_methods::AUTD3_TRUE;
  }

  template <group_f F>
  class GroupGuard {
   public:
    using key_type = typename std::invoke_result_t<F, const driver::geometry::Device&>::value_type;

    AUTD3_API explicit GroupGuard(F map, Controller& controller)
        : _controller(controller), _map(std::move(map)), _kv_map(native_methods::AUTDControllerGroupCreateKVMap()) {}

    template <driver::datagram D, typename Rep = uint64_t, typename Period = std::milli>
    AUTD3_API GroupGuard set(const key_type key, D&& data, const std::optional<std::chrono::duration<Rep, Period>> timeout = std::nullopt) {
      if (_keymap.contains(key)) throw AUTDException("Key already exists");
      const int64_t timeout_ns = timeout.has_value() ? timeout.value().count() : -1;
      const auto ptr = data.ptr(_controller._geometry);
      _keymap[key] = _k++;
      _kv_map = validate(native_methods::AUTDControllerGroupKVMapSet(_kv_map, _keymap[key], ptr, native_methods::DatagramPtr{nullptr}, timeout_ns));
      return std::move(*this);
    }

    template <driver::datagram D, typename Rep, typename Period>
    AUTD3_API GroupGuard set(const key_type key, D&& data, const std::chrono::duration<Rep, Period> timeout) {
      return set(key, std::forward<D>(data), std::optional(timeout));
    }

    template <driver::datagram D1, driver::datagram D2, typename Rep = uint64_t, typename Period = std::milli>
    AUTD3_API GroupGuard set(const key_type key, D1&& data1, D2&& data2,
                             const std::optional<std::chrono::duration<Rep, Period>> timeout = std::nullopt) {
      if (_keymap.contains(key)) throw AUTDException("Key already exists");
      const int64_t timeout_ns = timeout.has_value() ? timeout.value().count() : -1;
      const auto ptr1 = data1.ptr(_controller._geometry);
      const auto ptr2 = data2.ptr(_controller._geometry);
      _keymap[key] = _k++;
      _kv_map = validate(native_methods::AUTDControllerGroupKVMapSet(_kv_map, _keymap[key], ptr1, ptr2, timeout_ns));
      return std::move(*this);
    }

    template <driver::datagram D1, driver::datagram D2, typename Rep, typename Period>
    AUTD3_API GroupGuard set(const key_type key, D1&& data1, D2&& data2, const std::chrono::duration<Rep, Period> timeout) {
      return set(key, std::forward<D1>(data1), std::forward<D2>(data2), std::optional(timeout));
    }

    AUTD3_API bool send() {
      std::vector<int32_t> map;
      map.reserve(_controller.geometry().num_devices());
      std::transform(_controller.geometry().cbegin(), _controller.geometry().cend(), std::back_inserter(map),
                     [this](const driver::geometry::Device& d) {
                       if (!d.enable()) return -1;
                       const auto k = _map(d);
                       return k.has_value() ? _keymap[k.value()] : -1;
                     });
      return validate(AUTDControllerGroup(_controller._ptr, map.data(), _kv_map)) == native_methods::AUTD3_TRUE;
    }

#ifdef AUTD3_ASYNC_API
    AUTD3_API [[nodiscard]] coro::task<bool> send_async() { co_return send(); }
#endif

   private:
    Controller& _controller;
    F _map;
    native_methods::GroupKVMapPtr _kv_map;
    std::unordered_map<key_type, int32_t> _keymap;
    int32_t _k{0};
  };

  template <group_f F>
  AUTD3_API GroupGuard<F> group(const F& map) {
    return GroupGuard<F>(map, *this);
  }

#ifdef AUTD3_ASYNC_API
  /**
   * @brief Close connection
   */
  AUTD3_API [[nodiscard]] coro::task<bool> close_async() const { co_return close(); }
  /**
   * @brief Get FPGA information
   *
   * @return List of FPGA information
   */
  AUTD3_API [[nodiscard]] coro::task<std::vector<std::optional<driver::FPGAState>>> fpga_info_async() { co_return fpga_info(); }

  /**
   * @brief Send data to the devices
   *
   * @tparam D Datagram
   * @tparam Rep
   * @tparam Period
   * @param data data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D, typename Rep, typename Period>
  AUTD3_API [[nodiscard]] coro::task<bool> send_async(D&& data, const std::chrono::duration<Rep, Period> timeout) {
    auto res = co_await send_async(std::forward<D>(data), std::optional(timeout));
    co_return res;
  }

  /**
   * @brief Send data to the devices
   *
   * @tparam D Datagram
   * @tparam Rep
   * @tparam Period
   * @param data data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D, typename Rep = uint64_t, typename Period = std::milli>
  AUTD3_API [[nodiscard]] coro::task<bool> send_async(D&& data, const std::optional<std::chrono::duration<Rep, Period>> timeout = std::nullopt) {
    auto res = co_await send_async(std::forward<D>(data), driver::NullDatagram(), timeout);
    co_return res;
  }

  /**
   * @brief Send data to the devices
   *
   * @tparam D1 Datagram
   * @tparam D2 Datagram
   * @tparam Rep
   * @tparam Period
   * @param data1 first data
   * @param data2 second data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D1, driver::datagram D2, typename Rep, typename Period>
  AUTD3_API [[nodiscard]] coro::task<bool> send_async(D1&& data1, D2&& data2, const std::chrono::duration<Rep, Period> timeout) {
    auto res = co_await send_async(std::forward<D1>(data1), std::forward<D2>(data2), std::optional(timeout));
    co_return res;
  }

  /**
   * @brief Send data to the devices
   *
   * @tparam D1 Datagram
   * @tparam D2 Datagram
   * @tparam Rep
   * @tparam Period
   * @param data1 first data
   * @param data2 second data
   * @param timeout timeout
   * @return If true, it is confirmed that the data has been successfully transmitted. Otherwise, there are no errors, but it is unclear whether the
   * data has been sent reliably or not.
   */
  template <driver::datagram D1, driver::datagram D2, typename Rep = uint64_t, typename Period = std::milli>
  AUTD3_API [[nodiscard]] coro::task<bool> send_async(D1&& data1, D2&& data2,
                                                      const std::optional<std::chrono::duration<Rep, Period>> timeout = std::nullopt) {
    const int64_t timeout_ns = timeout.has_value() ? std::chrono::duration_cast<std::chrono::nanoseconds>(timeout.value()).count() : -1;
    co_return validate(AUTDControllerSend(_ptr, data1.ptr(_geometry), data2.ptr(_geometry), timeout_ns)) == native_methods::AUTD3_TRUE;
  }

  /**
   * @brief Get firmware information
   *
   * @return List of firmware information
   */
  AUTD3_API [[nodiscard]] coro::task<std::vector<driver::FirmwareInfo>> firmware_infos_async() { co_return firmware_infos(); }
#endif

 private:
  AUTD3_API Controller(driver::geometry::Geometry geometry, const native_methods::ControllerPtr ptr, L link)
      : _geometry(std::move(geometry)), _ptr(ptr), _link(std::move(link)) {}

  driver::geometry::Geometry _geometry;
  native_methods::ControllerPtr _ptr;
  L _link;
};

}  // namespace autd3::controller
