#include <gtest/gtest.h>

#include <autd3/driver/geometry/transducer.hpp>
#include <ranges>

#include "utils.hpp"

TEST(Internal_Geometry, TransducerIdx) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    std::ranges::for_each(std::views::iota(0) | std::views::take(dev.num_transducers()), [&dev](auto i) { ASSERT_EQ(dev[i].idx(), i); });
  }
}

TEST(Internal_Geometry, TransducerPosition) {
  auto autd = create_controller();

  ASSERT_EQ(autd.geometry()[0][0].position(), autd3::driver::Vector3(0, 0, 0));
  ASSERT_EQ(autd.geometry()[0][autd3::driver::AUTD3::NUM_TRANS_IN_UNIT - 1].position(), autd3::driver::Vector3(172.72, 132.08, 0));

  ASSERT_EQ(autd.geometry()[1][0].position(), autd3::driver::Vector3(0, 0, 0));
  ASSERT_EQ(autd.geometry()[1][autd3::driver::AUTD3::NUM_TRANS_IN_UNIT - 1].position(), autd3::driver::Vector3(172.72, 132.08, 0));
}

TEST(Internal_Geometry, TransducerRotation) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    std::ranges::for_each(dev.transducers(), [](auto& tr) { ASSERT_EQ(tr.rotation(), autd3::driver::Quaternion::Identity()); });
  }
}

TEST(Internal_Geometry, TransducerDirectionX) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    std::ranges::for_each(dev.transducers(), [](auto& tr) { ASSERT_EQ(tr.x_direction(), autd3::driver::Vector3::UnitX()); });
  }
}

TEST(Internal_Geometry, TransducerDirectionY) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    std::ranges::for_each(dev.transducers(), [](auto& tr) { ASSERT_EQ(tr.y_direction(), autd3::driver::Vector3::UnitY()); });
  }
}

TEST(Internal_Geometry, TransducerDirectionZ) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    std::ranges::for_each(dev.transducers(), [](auto& tr) { ASSERT_EQ(tr.z_direction(), autd3::driver::Vector3::UnitZ()); });
  }
}

TEST(Internal_Geometry, TransducerWavelength) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    for (auto& tr : dev) {
      ASSERT_DOUBLE_EQ(tr.wavelength(340e3), 340e3 / autd3::native_methods::ULTRASOUND_FREQUENCY);
    }
  }
}

TEST(Internal_Geometry, TransducerWavenum) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    for (auto& tr : dev) {
      ASSERT_DOUBLE_EQ(tr.wavenumber(340e3), 2 * autd3::driver::pi * autd3::native_methods::ULTRASOUND_FREQUENCY / 340e3);
    }
  }
}
