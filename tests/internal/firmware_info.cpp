#include <gtest/gtest.h>

#include <autd3/driver/firmware_version.hpp>
#include <autd3/exception.hpp>
#include <ranges>
#include <sstream>

#include "utils.hpp"

TEST(Internal, FirmwareInfo) {
  auto autd = create_controller();

  ASSERT_EQ("v5.1.1", autd3::driver::FirmwareInfo::latest_version());

  {
    const auto infos = autd.firmware_infos();
    std::ranges::for_each(std::ranges::views::iota(0) | std::ranges::views::take(infos.size()), [&](auto i) {
      std::stringstream ss;
      ss << i;
      ss << ": CPU = v5.1.1, FPGA = v5.1.1 [Emulator]";
      ASSERT_EQ(ss.str(), infos[i].info());
    });
  }

  {
    autd.link().break_down();
    ASSERT_THROW((void)autd.firmware_infos(), autd3::AUTDException);
  }
}

TEST(Internal, FirmwareInfoAsync) {
  auto autd = create_controller();

  ASSERT_EQ("v5.1.1", autd3::driver::FirmwareInfo::latest_version());

  {
    const auto infos = sync_wait(autd.firmware_infos_async());
    std::ranges::for_each(std::ranges::views::iota(0) | std::ranges::views::take(infos.size()), [&](auto i) {
      std::stringstream ss;
      ss << i;
      ss << ": CPU = v5.1.1, FPGA = v5.1.1 [Emulator]";
      ASSERT_EQ(ss.str(), infos[i].info());
    });
  }

  {
    autd.link().break_down();
    ASSERT_THROW((void)sync_wait(autd.firmware_infos_async()), autd3::AUTDException);
  }
}
