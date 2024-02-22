#include <gtest/gtest.h>

#include <autd3/driver/common/emit_intensity.hpp>
#include <autd3/gain/gain.hpp>

#include "utils.hpp"

class Uniform final : public autd3::gain::Gain<Uniform> {
 public:
  explicit Uniform(const uint8_t intensity, const uint8_t phase, std::vector<bool>* cnt)
      : _intensity(autd3::driver::EmitIntensity(intensity)), _phase(phase), _cnt(cnt) {}
  Uniform() = default;
  Uniform(const Uniform& obj) = default;
  Uniform& operator=(const Uniform& obj) = default;
  Uniform(Uniform&& obj) = default;
  Uniform& operator=(Uniform&& obj) = default;
  virtual ~Uniform() = default;

  AUTD3_API [[nodiscard]] std::unordered_map<size_t, std::vector<autd3::driver::Drive>> calc(
      const autd3::driver::geometry::Geometry& geometry) const override {
    return transform(geometry, [&](const auto& dev, const auto&) {
      _cnt->operator[](dev.idx()) = true;
      return autd3::driver::Drive{_phase, _intensity};
    });
  }

 private:
  autd3::driver::EmitIntensity _intensity;
  autd3::driver::Phase _phase;
  std::vector<bool>* _cnt;
};

TEST(Gain, Gain) {
  auto autd = create_controller();

  std::vector cnt(autd.geometry().num_devices(), false);
  ASSERT_TRUE(autd.send(Uniform(0x80, 0x90, &cnt)));

  for (auto& dev : autd.geometry()) {
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }
}

TEST(Gain, GainCheckOnlyForEnabled) {
  auto autd = create_controller();
  autd.geometry()[0].set_enable(false);

  std::vector check(autd.geometry().num_devices(), false);
  ASSERT_TRUE(autd.send(Uniform(0x80, 0x90, &check)));

  ASSERT_FALSE(check[0]);
  ASSERT_TRUE(check[1]);

  {
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
  {
    auto [intensities, phases] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }
}
