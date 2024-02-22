#include <gtest/gtest.h>

#include <autd3/driver/datagram/reads_fpga_state.hpp>
#include <autd3/driver/fpga/defined/fpga_state.hpp>
#include <autd3/exception.hpp>

#include "utils.hpp"

TEST(Internal, FPGAState) {
  auto autd = create_controller();

  {
    for (const auto infos = autd.fpga_info(); auto info : infos) ASSERT_FALSE(info.has_value());
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureReadsFPGAState([](const auto&) { return true; })));

  {
    autd.link().assert_thermal_sensor(0);

    const auto infos = autd.fpga_info();
    ASSERT_TRUE(infos[0].value().is_thermal_assert());
    ASSERT_FALSE(infos[1].value().is_thermal_assert());
  }

  {
    autd.link().deassert_thermal_sensor(0);
    autd.link().assert_thermal_sensor(1);

    const auto infos = autd.fpga_info();
    ASSERT_FALSE(infos[0].value().is_thermal_assert());
    ASSERT_TRUE(infos[1].value().is_thermal_assert());
  }

  {
    autd.link().break_down();
    ASSERT_THROW((void)autd.fpga_info(), autd3::AUTDException);
  }
}

TEST(Internal, FPGAStateAsync) {
  auto autd = create_controller();

  {
    for (const auto infos = autd.fpga_info(); auto info : infos) ASSERT_FALSE(info.has_value());
  }

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureReadsFPGAState([](const auto&) { return true; })));

  {
    autd.link().assert_thermal_sensor(0);

    const auto infos = sync_wait(autd.fpga_info_async());
    ASSERT_TRUE(infos[0].value().is_thermal_assert());
    ASSERT_FALSE(infos[1].value().is_thermal_assert());
  }

  {
    autd.link().deassert_thermal_sensor(0);
    autd.link().assert_thermal_sensor(1);

    const auto infos = sync_wait(autd.fpga_info_async());
    ASSERT_FALSE(infos[0].value().is_thermal_assert());
    ASSERT_TRUE(infos[1].value().is_thermal_assert());
  }

  {
    autd.link().break_down();
    ASSERT_THROW((void)sync_wait(autd.fpga_info_async()), autd3::AUTDException);
  }
}
