#pragma once

#include <string>

#include "autd3/native_methods.hpp"

namespace autd3::driver {

class FirmwareInfo {
 public:
  explicit FirmwareInfo(std::string info) noexcept : _info(std::move(info)) {}

  AUTD3_API [[nodiscard]] std::string info() const { return _info; }

  AUTD3_API [[nodiscard]] static std::string latest_version() {
    char info[256];
    native_methods::AUTDFirmwareLatest(info);
    return {info};
  }

 private:
  std::string _info;
};

AUTD3_API inline std::ostream& operator<<(std::ostream& os, const FirmwareInfo& obj) {
  os << obj.info();
  return os;
}

}  // namespace autd3::driver
