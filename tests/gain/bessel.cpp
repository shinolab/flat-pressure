#include <gtest/gtest.h>

#include <autd3/gain/bessel.hpp>

#include "utils.hpp"

TEST(Gain, Bessel) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::Bessel(autd.geometry().center(), autd3::driver::Vector3::UnitZ(), autd3::driver::pi / 4).with_intensity(0x80)));

  for (auto& dev : autd.geometry()) {
    auto [intensities, phases] = autd.link().intensities_and_phases(dev.idx(), 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::any_of(phases, [](auto p) { return p != 0; }));
  }
}

TEST(Gain, BesselDefault) {
  auto g = autd3::gain::Bessel(autd3::driver::Vector3::Zero(), autd3::driver::Vector3::UnitZ(), 0);
  ASSERT_EQ(g.intensity().value(), autd3::native_methods::AUTDGainBesselDefaultIntensity());
}
