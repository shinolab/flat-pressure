#pragma once

#include <chrono>
#include <string>

#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::controller {
class ControllerBuilder;
}

namespace autd3::link {

/**
 * @brief Link using TwinCAT3
 */
class TwinCAT final {
  TwinCAT() = default;

 public:
  class Builder final {
    friend class TwinCAT;
    friend class controller::ControllerBuilder;

    native_methods::LinkTwinCATBuilderPtr _ptr;

    Builder() : _ptr(native_methods::AUTDLinkTwinCAT()) {}

    AUTD3_API [[nodiscard]] static TwinCAT resolve_link(native_methods::LinkPtr) { return TwinCAT{}; }

   public:
    using Link = TwinCAT;

    AUTD3_API [[nodiscard]] native_methods::LinkBuilderPtr ptr() const { return AUTDLinkTwinCATIntoBuilder(_ptr); }

    template <typename Rep, typename Period>
    AUTD3_API [[nodiscard]] Builder with_timeout(const std::chrono::duration<Rep, Period> timeout) {
      const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count();
      _ptr = AUTDLinkTwinCATWithTimeout(_ptr, static_cast<uint64_t>(ns));
      return *this;
    }
  };

  AUTD3_API [[nodiscard]] static Builder builder() { return {}; }
};

/**
 * @brief Link for remote TwinCAT3 server via [ADS](https://github.com/Beckhoff/ADS) library
 */
class RemoteTwinCAT final {
  RemoteTwinCAT() = default;

 public:
  class Builder final {
    friend class RemoteTwinCAT;
    friend class controller::ControllerBuilder;

    native_methods::LinkRemoteTwinCATBuilderPtr _ptr;

    AUTD3_API [[nodiscard]] explicit Builder(const std::string& server_ams_net_id) {
      _ptr = validate(native_methods::AUTDLinkRemoteTwinCAT(server_ams_net_id.c_str()));
    }

    AUTD3_API [[nodiscard]] static RemoteTwinCAT resolve_link(native_methods::LinkPtr) { return RemoteTwinCAT{}; }

   public:
    using Link = RemoteTwinCAT;

    AUTD3_API [[nodiscard]] native_methods::LinkBuilderPtr ptr() const { return AUTDLinkRemoteTwinCATIntoBuilder(_ptr); }

    /**
     * @brief Set server IP address
     *
     * @param ip Server IP address
     * @return RemoteTwinCAT
     */
    AUTD3_API [[nodiscard]] Builder with_server_ip(const std::string& ip) {
      _ptr = AUTDLinkRemoteTwinCATWithServerIP(_ptr, ip.c_str());
      return *this;
    }

    /**
     * @brief Set client AMS Net ID
     *
     * @param id AMS Net ID
     * @return RemoteTwinCAT
     */
    AUTD3_API [[nodiscard]] Builder with_client_ams_net_id(const std::string& id) {
      _ptr = AUTDLinkRemoteTwinCATWithClientAmsNetId(_ptr, id.c_str());
      return *this;
    }

    template <typename Rep, typename Period>
    AUTD3_API [[nodiscard]] Builder with_timeout(const std::chrono::duration<Rep, Period> timeout) {
      const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count();
      _ptr = AUTDLinkRemoteTwinCATWithTimeout(_ptr, static_cast<uint64_t>(ns));
      return *this;
    }
  };

  /**
   * @brief Constructor
   *
   * @param server_ams_net_id Server AMS Net ID
   */
  AUTD3_API [[nodiscard]] static Builder builder(const std::string& server_ams_net_id) { return Builder(server_ams_net_id); }
};

}  // namespace autd3::link
