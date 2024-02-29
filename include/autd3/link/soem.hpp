#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::controller {
class ControllerBuilder;
}

namespace autd3::link {

using native_methods::SyncMode;

class EtherCATAdapter {
  std::string _desc;
  std::string _name;

 public:
  EtherCATAdapter(std::string desc, std::string name) : _desc(std::move(desc)), _name(std::move(name)) {}

  [[nodiscard]] const std::string& desc() const { return _desc; }
  [[nodiscard]] const std::string& name() const { return _name; }
};

using native_methods::Status;

template <class F>
concept soem_err_handler_f = requires(F f, const uint16_t slave, const Status status, const std::string& msg) {
  { f(slave, status, msg) } -> std::same_as<void>;
};

/**
 * @brief Link using [SOEM](https://github.com/OpenEtherCATsociety/SOEM)
 *
 */
class SOEM final {
  using native_err_handler_t = void (*)(const void*, uint32_t, uint8_t, const char*);
  using err_handler_t = void (*)(uint16_t, Status, const std::string&);

  explicit SOEM(const native_err_handler_t native_err_handler, const err_handler_t err_handler)
      : _native_err_handler(native_err_handler), _err_handler(err_handler) {}

  native_err_handler_t _native_err_handler;
  err_handler_t _err_handler;

 public:
  class Builder final {
    friend class SOEM;
    friend class controller::ControllerBuilder;

    native_methods::LinkSOEMBuilderPtr _ptr;
    native_err_handler_t _native_err_handler;
    err_handler_t _err_handler;

    Builder() : _ptr(native_methods::AUTDLinkSOEM()), _err_handler(nullptr) {}

    [[nodiscard]] SOEM resolve_link(native_methods::LinkPtr) const { return SOEM{_native_err_handler, _err_handler}; }

   public:
    using Link = SOEM;

    [[nodiscard]] native_methods::LinkBuilderPtr ptr() const { return AUTDLinkSOEMIntoBuilder(_ptr); }

    /**
     * @brief Set network interface name
     * @details If empty, this link will automatically find the network
     * interface that is connected to AUTD3 devices.
     *
     * @param ifname Network interface name
     * @return Builder
     */
    [[nodiscard]] Builder with_ifname(const std::string& ifname) {
      _ptr = AUTDLinkSOEMWithIfname(_ptr, ifname.c_str());
      return *this;
    }

    /**
     * @brief Set buffer size
     *
     * @param value
     * @return Builder
     */
    [[nodiscard]] Builder with_buf_size(const size_t value) {
      _ptr = AUTDLinkSOEMWithBufSize(_ptr, static_cast<uint32_t>(value));
      return *this;
    }

    /**
     * @brief Set send cycle (the unit is 500us)
     *
     * @param value
     * @return Builder
     */
    [[nodiscard]] Builder with_send_cycle(const uint16_t value) {
      _ptr = AUTDLinkSOEMWithSendCycle(_ptr, value);
      return *this;
    }

    /**
     * @brief Set sync0 cycle (the unit is 500us)
     *
     * @param value
     * @return Builder
     */
    [[nodiscard]] Builder with_sync0_cycle(const uint16_t value) {
      _ptr = AUTDLinkSOEMWithSync0Cycle(_ptr, value);
      return *this;
    }

    /**
     * @brief Set callback function when some error occur
     *
     * @param value
     * @return Builder
     */
    template <soem_err_handler_f F>
    [[nodiscard]] Builder with_err_handler(F value) {
      _err_handler = static_cast<err_handler_t>(value);
      _native_err_handler = +[](const void* context, const uint32_t slave, const uint8_t status, const char* msg) {
        (*reinterpret_cast<err_handler_t>(const_cast<void*>(context)))(static_cast<uint16_t>(slave), static_cast<Status>(status),  // LCOV_EXCL_LINE
                                                                       std::string(msg));                                          // LCOV_EXCL_LINE
      };
      _ptr = AUTDLinkSOEMWithErrHandler(_ptr, reinterpret_cast<void*>(_native_err_handler), reinterpret_cast<void*>(_err_handler));
      return *this;
    }

    /**
     * @brief Set timer strategy
     *
     * @param value
     * @return Builder
     */
    [[nodiscard]] Builder with_timer_strategy(const native_methods::TimerStrategy value) {
      _ptr = AUTDLinkSOEMWithTimerStrategy(_ptr, value);
      return *this;
    }

    /**
     * @brief Set sync mode
     * @details See [Beckhoff's site](https://infosys.beckhoff.com/content/1033/ethercatsystem/2469122443.html) for more details.
     *
     * @param value
     * @return Builder
     */
    [[nodiscard]] Builder with_sync_mode(const SyncMode value) {
      _ptr = AUTDLinkSOEMWithSyncMode(_ptr, value);
      return *this;
    }

    /**
     * @brief Set state check interval
     *
     * @param value
     * @return Builder
     */
    template <typename Rep, typename Period>
    [[nodiscard]] Builder with_state_check_interval(const std::chrono::duration<Rep, Period> value) {
      const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(value).count();
      _ptr = AUTDLinkSOEMWithStateCheckInterval(_ptr, static_cast<uint32_t>(ms));
      return *this;
    }

    template <typename Rep, typename Period>
    [[nodiscard]] Builder with_timeout(const std::chrono::duration<Rep, Period> timeout) {
      const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count();
      _ptr = AUTDLinkSOEMWithTimeout(_ptr, static_cast<uint64_t>(ns));
      return *this;
    }
  };

  static Builder builder() { return {}; }

  [[nodiscard]] static std::vector<EtherCATAdapter> enumerate_adapters() {
    const auto handle = native_methods::AUTDAdapterPointer();
    const auto len = AUTDAdapterGetSize(handle);
    std::vector<EtherCATAdapter> adapters;
    for (uint32_t i = 0; i < len; i++) {
      char sb_desc[128];
      char sb_name[128];
      AUTDAdapterGetAdapter(handle, i, sb_desc, sb_name);
      adapters.emplace_back(std::string(sb_desc), std::string(sb_name));
    }
    AUTDAdapterPointerDelete(handle);
    return adapters;
  }
};

/**
 * @brief Link to connect to remote SOEMServer
 */
class RemoteSOEM final {
  RemoteSOEM() = default;

 public:
  class Builder final {
    friend class RemoteSOEM;
    friend class controller::ControllerBuilder;

    native_methods::LinkRemoteSOEMBuilderPtr _ptr;

    explicit Builder(const std::string& addr) { _ptr = validate(native_methods::AUTDLinkRemoteSOEM(addr.c_str())); }

    [[nodiscard]] static RemoteSOEM resolve_link(native_methods::LinkPtr) { return RemoteSOEM{}; }

   public:
    using Link = RemoteSOEM;

    [[nodiscard]] native_methods::LinkBuilderPtr ptr() const { return AUTDLinkRemoteSOEMIntoBuilder(_ptr); }

    template <typename Rep, typename Period>
    [[nodiscard]] Builder with_timeout(const std::chrono::duration<Rep, Period> timeout) {
      const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count();
      _ptr = AUTDLinkRemoteSOEMWithTimeout(_ptr, static_cast<uint64_t>(ns));
      return std::move(*this);
    }
  };

  /**
   * @brief Constructor
   *
   * @param addr IP address and port of SOEMServer (e.g., "127.0.0.1:8080")
   */
  [[nodiscard]] static Builder builder(const std::string& addr) { return Builder(addr); }
};

}  // namespace autd3::link
