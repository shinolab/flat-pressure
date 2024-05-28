#include <gtest/gtest.h>

#include <autd3/driver/datagram/datagram.hpp>
#include <autd3/driver/geometry/device.hpp>
#include <ranges>

#include "utils.hpp"

TEST(DriverGeomtry, DeviceIdx) {
  auto autd = create_controller();
  ASSERT_EQ(autd.geometry()[0].idx(), 0);
  ASSERT_EQ(autd.geometry()[1].idx(), 1);
}

TEST(DriverGeomtry, DeviceSoundSpeed) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    ASSERT_EQ(dev.sound_speed(), 340e3);
    dev.set_sound_speed(350e3);
    ASSERT_EQ(dev.sound_speed(), 350e3);
  }
}

TEST(DriverGeomtry, DeviceSoundSpeedFromTemp) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    dev.set_sound_speed_from_temp(15);
    ASSERT_EQ(dev.sound_speed(), 340.2952640537549e3);
  }
}

TEST(DriverGeomtry, DeviceAttenuation) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    ASSERT_EQ(dev.attenuation(), 0);
    dev.set_attenuation(1);
    ASSERT_EQ(dev.attenuation(), 1);
  }
}

TEST(DriverGeomtry, DeviceNumTransducers) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    ASSERT_EQ(dev.num_transducers(), 249);
  }
}

TEST(DriverGeomtry, DeviceCenter) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    ASSERT_NEAR_VECTOR3(dev.center(), autd3::driver::Vector3(86.62522088353406, 66.7132530125621, 0), 1e-6);
  }
}

TEST(DriverGeomtry, DeviceEnable) {
  auto autd = create_controller();
  for (auto& dev : autd.geometry()) ASSERT_TRUE(dev.enable());

  autd.geometry()[0].set_enable(true);
  autd.geometry()[1].set_enable(false);
  ASSERT_TRUE(autd.geometry()[0].enable());
  ASSERT_FALSE(autd.geometry()[1].enable());
}

TEST(DriverGeomtry, DeviceTranslate) {
  for (auto autd = create_controller(); const auto& dev : autd.geometry()) {
    auto original_pos_view = dev.transducers() | std::views::transform([](const auto& tr) { return tr.position(); });
    std::vector original_pos(original_pos_view.begin(), original_pos_view.end());
    autd3::driver::Vector3 t(1, 2, 3);
    dev.translate(t);
    std::ranges::for_each(dev.transducers(), [&t, &original_pos](auto& tr) { ASSERT_EQ(tr.position(), original_pos[tr.idx()] + t); });
  }
}

TEST(DriverGeomtry, DeviceRotate) {
  for (auto autd = create_controller(); const auto& dev : autd.geometry()) {
    autd3::driver::Quaternion r(0.7071067811865476, 0, 0, 0.7071067811865476);
    dev.rotate(r);
    std::ranges::for_each(dev.transducers(), [&r](auto& tr) { ASSERT_EQ(tr.rotation(), r); });
  }
}

TEST(DriverGeomtry, DeviceAffine) {
  for (auto autd = create_controller(); const auto& dev : autd.geometry()) {
    auto original_pos_view = dev.transducers() | std::views::transform([](const auto& tr) { return tr.position(); });
    std::vector original_pos(original_pos_view.begin(), original_pos_view.end());
    autd3::driver::Vector3 t(1, 2, 3);
    autd3::driver::Quaternion r(0.7071067811865476, 0, 0, 0.7071067811865476);
    dev.affine(t, r);
    std::ranges::for_each(dev.transducers(), [&r, &t, &original_pos](auto& tr) {
      auto op = original_pos[tr.idx()];
      autd3::driver::Vector3 expected = autd3::driver::Vector3(-op.y(), op.x(), op.z()) + t;
      ASSERT_DOUBLE_EQ(tr.position().x(), expected.x());
      ASSERT_DOUBLE_EQ(tr.position().y(), expected.y());
      ASSERT_DOUBLE_EQ(tr.position().z(), expected.z());
      ASSERT_EQ(tr.rotation(), r);
    });
  }
}

TEST(DriverGeomtry, TransducerLocal) {
  for (auto autd = create_controller(); auto& dev : autd.geometry()) {
    std::ranges::for_each(std::views::iota(0) | std::views::take(dev.num_transducers()), [&dev](auto i) { ASSERT_EQ(dev[i].idx(), i); });
  }
}
