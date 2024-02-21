#include <gtest/gtest.h>

#include <autd3/driver/common/emit_intensity.hpp>
#include <autd3/gain/cache.hpp>
#include <autd3/gain/gain.hpp>
#include <autd3/gain/uniform.hpp>

#include "utils.hpp"

TEST(Gain, Cache) {
  auto autd = create_controller();

  const auto g = autd3::gain::Uniform(0x80).with_phase(autd3::driver::Phase(0x90)).with_cache();

  g.init(autd.geometry());

  ASSERT_TRUE(autd.send(g));
  for (auto& dev : autd.geometry()) {
    ASSERT_TRUE(std::ranges::all_of(g.drives().at(dev.idx()), [](auto d) { return d == autd3::driver::Drive{autd3::driver::Phase(0x90), 0x80}; }));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }
}

class ForCacheTest final : public autd3::gain::Gain, public autd3::gain::IntoCache<ForCacheTest> {
 public:
  explicit ForCacheTest(size_t* cnt) : _cnt(cnt) {}

  [[nodiscard]] std::unordered_map<size_t, std::vector<autd3::driver::Drive>> calc(const autd3::driver::geometry::Geometry& geometry) const override {
    ++*_cnt;
    return transform(geometry, [&](const auto&, const auto&) {
      return autd3::driver::Drive{autd3::driver::Phase(0x90), autd3::driver::EmitIntensity(0x80)};
    });
  }

 private:
  size_t* _cnt;
};

TEST(Gain, CacheCheckOnce) {
  auto autd = create_controller();

  {
    size_t cnt = 0;
    ForCacheTest g(&cnt);
    ASSERT_TRUE(autd.send(g));
    ASSERT_EQ(cnt, 1);
    ASSERT_TRUE(autd.send(g));
    ASSERT_EQ(cnt, 2);
  }

  {
    size_t cnt = 0;
    ForCacheTest g(&cnt);
    auto gc = g.with_cache();
    ASSERT_EQ(cnt, 0);
    gc.init(autd.geometry());
    ASSERT_EQ(cnt, 1);
    ASSERT_TRUE(autd.send(gc));
    ASSERT_EQ(cnt, 1);
    ASSERT_TRUE(autd.send(gc));
    ASSERT_EQ(cnt, 1);
  }
}

TEST(Gain, CacheCheckOnlyForEnabled) {
  auto autd = create_controller();
  autd.geometry()[0].set_enable(false);

  size_t cnt = 0;
  auto g = ForCacheTest(&cnt).with_cache();
  ASSERT_TRUE(autd.send(g));

  ASSERT_FALSE(g.drives().contains(0));
  ASSERT_TRUE(g.drives().contains(1));

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
