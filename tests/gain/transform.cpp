#include <gtest/gtest.h>

#include <autd3/gain/uniform.hpp>

#include "utils.hpp"

TEST(Gain, Transform) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::gain::Uniform(0x80)
                            .with_phase(autd3::driver::Phase(128))
                            .with_transform([](const autd3::driver::geometry::Device& dev, const autd3::driver::geometry::Transducer&,
                                               const autd3::driver::Drive d) -> autd3::driver::Drive {
                              if (dev.idx() == 0) {
                                return autd3::driver::Drive{autd3::driver::Phase(d.phase.value() + 32), d.intensity};
                              }
                              return autd3::driver::Drive{autd3::driver::Phase(d.phase.value() - 32), d.intensity};
                            })));

  {
    auto [intensities, phases] = autd.link().drives(0, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 128 + 32; }));
  }

  {
    auto [intensities, phases] = autd.link().drives(1, autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 128 - 32; }));
  }
}

TEST(Gain, TransformCheckOnlyForEnabled) {
  auto autd = create_controller();
  autd.geometry()[0].set_enable(false);

  std::vector cnt(autd.geometry().num_devices(), false);
  ASSERT_TRUE(autd.send(autd3::gain::Uniform(0x80)
                            .with_phase(autd3::driver::Phase(0x90))
                            .with_transform([&cnt](const autd3::driver::geometry::Device& dev, const autd3::driver::geometry::Transducer&,
                                                   const autd3::driver::Drive d) -> autd3::driver::Drive {
                              cnt[dev.idx()] = true;
                              return d;
                            })));

  ASSERT_FALSE(cnt[0]);
  ASSERT_TRUE(cnt[1]);

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
