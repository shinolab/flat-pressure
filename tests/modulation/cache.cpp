#include <gtest/gtest.h>

#include <autd3/modulation/modulation.hpp>
#include <autd3/modulation/static.hpp>

#include "utils.hpp"

TEST(Modulation, Cache) {
  auto autd1 = create_controller();
  auto autd2 = create_controller();

  const auto m1 = autd3::modulation::Static::with_intensity(0x80);
  const auto m2 = autd3::modulation::Static::with_intensity(0x80).with_cache();

  ASSERT_TRUE(autd1.send(m1));
  ASSERT_TRUE(autd2.send(m2));

  ASSERT_TRUE(std::ranges::all_of(m2.buffer(), [](auto d) { return d == autd3::driver::EmitIntensity(0x80); }));
  for (const auto& m : m2) ASSERT_EQ(autd3::driver::EmitIntensity(0x80), m);
  std::for_each(m2.cbegin(), m2.cend(), [](const auto& m) { ASSERT_EQ(autd3::driver::EmitIntensity(0x80), m); });
  for (size_t i = 0; i < m2.size(); i++) ASSERT_EQ(autd3::driver::EmitIntensity(0x80), m2[i]);
  for (auto& dev : autd1.geometry()) {
    auto mod = autd2.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    auto mod_expect = autd1.link().modulation(dev.idx(), autd3::native_methods::Segment::S0);
    ASSERT_TRUE(std::ranges::equal(mod, mod_expect));
    ASSERT_EQ(0xFFFFFFFF, autd2.link().modulation_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }
}

class ForModulationCacheTest final : public autd3::modulation::Modulation<ForModulationCacheTest>,
                                     public autd3::modulation::IntoCache<ForModulationCacheTest> {
 public:
  AUTD3_API [[nodiscard]] std::vector<autd3::driver::EmitIntensity> calc() const override {
    ++*_cnt;
    return {autd3::driver::EmitIntensity::maximum(), autd3::driver::EmitIntensity::maximum()};
  }

  explicit ForModulationCacheTest(size_t* cnt) noexcept
      : Modulation(autd3::driver::SamplingConfiguration::from_frequency_division(5120)), _cnt(cnt) {}

 private:
  size_t* _cnt;
};

TEST(Modulation, CacheCheckOnce) {
  auto autd = create_controller();

  {
    size_t cnt = 0;
    ForModulationCacheTest m(&cnt);
    ASSERT_TRUE(autd.send(m));
    ASSERT_EQ(cnt, 1);
    ASSERT_TRUE(autd.send(m));
    ASSERT_EQ(cnt, 2);
  }

  {
    size_t cnt = 0;
    ForModulationCacheTest g(&cnt);
    auto gc = g.with_cache();
    ASSERT_TRUE(autd.send(gc));
    ASSERT_EQ(cnt, 1);
    ASSERT_TRUE(autd.send(gc));
    ASSERT_EQ(cnt, 1);
  }
}
