#include <gtest/gtest.h>

#include <autd3/driver/datagram/debug.hpp>

#include "utils.hpp"

TEST(DriverDatagram, ConfigureDebugOutputIdx) {
  auto autd = create_controller();

  for (auto& dev : autd.geometry()) {
    std::array<uint8_t, 4> ty{0x00, 0x00, 0x00, 0x00};
    std::array<uint16_t, 4> value{0x0000, 0x0000, 0x0000, 0x0000};
    ASSERT_EQ(ty, autd.link().debug_types(dev.idx()));
    ASSERT_EQ(value, autd.link().debug_values(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureDebugSettings([](const autd3::driver::geometry::Device&) -> std::array<autd3::driver::DebugTypes, 4> {
    return {
        autd3::driver::DebugType::None,
        autd3::driver::DebugType::BaseSignal,
        autd3::driver::DebugType::Thermo,
        autd3::driver::DebugType::ForceFan,
    };
  })));
  for (auto& dev : autd.geometry()) {
    std::array<uint8_t, 4> ty{0x00, 0x01, 0x02, 0x03};
    std::array<uint16_t, 4> value{0x0000, 0x0000, 0x0000, 0x0000};
    ASSERT_EQ(ty, autd.link().debug_types(dev.idx()));
    ASSERT_EQ(value, autd.link().debug_values(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureDebugSettings([](const autd3::driver::geometry::Device&)->std::array<autd3::driver::DebugTypes, 4> {
      return {
          autd3::driver::DebugType::Sync,
          autd3::driver::DebugType::ModSegment,
          autd3::driver::DebugType::ModIdx(0x01),
          autd3::driver::DebugType::StmSegment,
      };
  })));
  for (auto& dev : autd.geometry()) {
      std::array<uint8_t, 4> ty{ 0x10, 0x20, 0x21, 0x50 };
      std::array<uint16_t, 4> value{ 0x0000, 0x0000, 0x0001, 0x0000 };
      ASSERT_EQ(ty, autd.link().debug_types(dev.idx()));
      ASSERT_EQ(value, autd.link().debug_values(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureDebugSettings([](const autd3::driver::geometry::Device& dev)->std::array<autd3::driver::DebugTypes, 4> {
      return {
          autd3::driver::DebugType::StmIdx(0x02),
          autd3::driver::DebugType::IsStmMode,
          autd3::driver::DebugType::PwmOut(&dev[3]),
          autd3::driver::DebugType::Direct(true),
      };
  })));
  for (auto& dev : autd.geometry()) {
      std::array<uint8_t, 4> ty{ 0x51, 0x52, 0xE0, 0xF0 };
      std::array<uint16_t, 4> value{ 0x0002, 0x0000, 0x0003, 0x0001 };
      ASSERT_EQ(ty, autd.link().debug_types(dev.idx()));
      ASSERT_EQ(value, autd.link().debug_values(dev.idx()));
  }
}
