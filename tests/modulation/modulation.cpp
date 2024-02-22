#include <gtest/gtest.h>

#include <autd3/modulation/modulation.hpp>

#include "utils.hpp"

class BurstModulation final : public autd3::modulation::Modulation<BurstModulation> {
 public:
  AUTD3_API [[nodiscard]] std::vector<autd3::driver::EmitIntensity> calc() const override {
    std::vector buffer(10, autd3::driver::EmitIntensity::minimum());
    buffer[0] = autd3::driver::EmitIntensity::maximum();
    return buffer;
  }

  explicit BurstModulation() noexcept : Modulation(autd3::driver::SamplingConfiguration::from_frequency_division(5120)) {}
};

TEST(Modulation, Modulation) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(BurstModulation()));

  for (auto& dev : autd.geometry()) {
    auto mod = autd.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    std::vector<uint8_t> mod_expect{255, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(5120, autd.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }
}
