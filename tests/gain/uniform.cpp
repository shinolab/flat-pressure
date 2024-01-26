#include <gtest/gtest.h>

#include <autd3/gain/uniform.hpp>

#include "utils.hpp"

TEST(Gain, Uniform) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::Uniform(0x80).with_phase(autd3::driver::Phase(0x90))));

  for (auto& dev : autd.geometry()) {
    auto [intensities, phases] = autd.link().intensities_and_phases(dev.idx(), 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }
}

TEST(Gain, UniformDefault) {
  auto autd = create_controller();
  auto g = autd3::gain::Uniform(0x00);
  ASSERT_TRUE(autd3::native_methods::AUTDGainUniformIsDefault(g.gain_ptr(autd.geometry())));
}
