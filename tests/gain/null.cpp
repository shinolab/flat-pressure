#include <gtest/gtest.h>

#include <autd3/gain/null.hpp>

#include "utils.hpp"

TEST(Gain, Null) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::Null()));

  for (auto& dev : autd.geometry()) {
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
}
