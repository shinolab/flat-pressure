#include <gtest/gtest.h>

#include <autd3/driver/datagram/reads_fpga_state.hpp>
#include <autd3/modulation/static.hpp>

#include "utils.hpp"

using autd3::native_methods::Segment;

TEST(DriverDatagramModulation, Segment) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureReadsFPGAState([](const auto&) { return true; })));

  auto infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_mod_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_mod_segment(dev.idx()));
    ASSERT_TRUE(std::ranges::all_of(autd.link().modulation(dev.idx(), Segment::S0), [](auto d) { return d == 0xFF; }));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Static::with_intensity(0x80)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_mod_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_mod_segment(dev.idx()));
    ASSERT_TRUE(std::ranges::all_of(autd.link().modulation(dev.idx(), Segment::S0), [](auto d) { return d == 0x80; }));
  }

  auto m = autd3::modulation::Static::with_intensity(0x81);
  ASSERT_TRUE(autd.send(m.with_segment(Segment::S1, true)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(Segment::S1, infos[dev.idx()].value().current_mod_segment());
    ASSERT_EQ(Segment::S1, autd.link().current_mod_segment(dev.idx()));
    ASSERT_TRUE(std::ranges::all_of(autd.link().modulation(dev.idx(), Segment::S1), [](auto d) { return d == 0x81; }));
  }

  ASSERT_TRUE(autd.send(autd3::modulation::Static::with_intensity(0x82).with_segment(Segment::S0, false)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(Segment::S1, infos[dev.idx()].value().current_mod_segment());
    ASSERT_EQ(Segment::S1, autd.link().current_mod_segment(dev.idx()));
    ASSERT_TRUE(std::ranges::all_of(autd.link().modulation(dev.idx(), Segment::S0), [](auto d) { return d == 0x82; }));
    ASSERT_TRUE(std::ranges::all_of(autd.link().modulation(dev.idx(), Segment::S1), [](auto d) { return d == 0x81; }));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ChangeModulationSegment(Segment::S0)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_mod_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_mod_segment(dev.idx()));
  }
}
