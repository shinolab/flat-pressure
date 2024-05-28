#include <gtest/gtest.h>

#include <autd3/gain/plane.hpp>

#include "utils.hpp"

TEST(Gain, Plane) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::Plane(autd3::driver::Vector3::UnitZ()).with_intensity(0x80).with_phase(autd3::driver::Phase(0x81))));

  for (auto& dev : autd.geometry()) {
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x81; }));
  }
}

TEST(Gain, PlaneDefault) {
  auto autd = create_controller();
  auto g = autd3::gain::Plane(autd3::driver::Vector3::UnitZ());
  ASSERT_TRUE(autd3::native_methods::AUTDGainPlanelIsDefault(g.gain_ptr(autd.geometry())));
}
