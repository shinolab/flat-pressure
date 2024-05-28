#include <gtest/gtest.h>

#include <autd3/modulation/fourier.hpp>
#include <ranges>

#include "utils.hpp"

TEST(Modulation, Fourier) {
  auto autd = create_controller();

  std::vector f{200};
  autd3::modulation::Fourier m =
      (autd3::modulation::Sine(50) + autd3::modulation::Sine(100))
          .add_component(autd3::modulation::Sine(150))
          .add_components_from_iter(f | std::ranges::views::transform([](const auto x) { return autd3::modulation::Sine(x); })) +
      autd3::modulation::Sine(250);

  ASSERT_TRUE(autd.send(m));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    std::vector<uint8_t> mod_expect{
        127, 156, 183, 205, 220, 227, 226, 219, 205, 188, 170, 153, 139, 129, 124, 123, 127, 133, 140, 147, 152, 155, 154, 151, 145, 138, 131,
        125, 120, 118, 119, 122, 127, 132, 137, 140, 141, 141, 137, 133, 127, 121, 116, 113, 112, 113, 117, 121, 127, 131, 134, 135, 133, 129,
        122, 115, 108, 102, 99,  99,  101, 106, 113, 120, 127, 130, 129, 124, 115, 100, 83,  65,  48,  35,  27,  26,  34,  48,  70,  97,
    };
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }
}
