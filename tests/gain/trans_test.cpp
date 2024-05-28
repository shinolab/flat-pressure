#include <gtest/gtest.h>

#include <autd3/gain/trans_test.hpp>
#include <ranges>

#include "utils.hpp"

TEST(Gain, TransTest) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::TransducerTest(
      [](const autd3::driver::geometry::Device& dev, const autd3::driver::geometry::Transducer& tr) -> std::optional<autd3::driver::Drive> {
        if (dev.idx() == 0 && tr.idx() == 0) return autd3::driver::Drive(autd3::driver::Phase(0x90), 0x80);
        if (dev.idx() == 1 && tr.idx() == 248) return autd3::driver::Drive(autd3::driver::Phase(0x91), 0x81);
        return std::nullopt;
      })));

  {
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_EQ(0x80, intensities[0]);
    ASSERT_EQ(0x90, phases[0]);
    ASSERT_TRUE(std::ranges::all_of(intensities | std::ranges::views::drop(1), [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases | std::ranges::views::drop(1), [](auto p) { return p == 0; }));
  }

  {
    auto [intensities, phases] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
    const auto idx = autd.geometry()[1].num_transducers() - 1;
    ASSERT_EQ(0x81, intensities[idx]);
    ASSERT_EQ(0x91, phases[idx]);
    ASSERT_TRUE(std::ranges::all_of(intensities | std::ranges::views::take(idx), [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases | std::ranges::views::take(idx), [](auto p) { return p == 0; }));
  }
}
