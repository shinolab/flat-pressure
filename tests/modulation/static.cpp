#include <gtest/gtest.h>

#include <autd3/modulation/static.hpp>

#include "utils.hpp"

TEST(Modulation, Static) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::modulation::Static().with_intensity(32)));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx());
    std::vector<uint8_t> mod_expect{32, 32};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(0xFFFFFFFF, autd.link().modulation_frequency_division(dev.idx()));
  }
}

TEST(Modulation, StaticDefault) {
  const auto m = autd3::modulation::Static();
  ASSERT_EQ(m.intensity().value(), autd3::native_methods::AUTDModulationStaticDefaultIntensity());
}
