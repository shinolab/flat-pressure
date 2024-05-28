#include <gtest/gtest.h>

#include <autd3/driver/datagram/reads_fpga_state.hpp>
#include <autd3/gain/gain.hpp>
#include <autd3/gain/uniform.hpp>

#include "utils.hpp"

using autd3::native_methods::Segment;

TEST(DriverDatagramGain, Segment) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureReadsFPGAState([](const auto&) { return true; })));

  auto infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_stm_segment(dev.idx()));
    auto [intensities, phases] = autd.link().drives(dev.idx(), Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x00; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x00; }));
  }

  ASSERT_TRUE(autd.send(autd3::gain::Uniform(0x80).with_phase(autd3::driver::Phase(0x90))));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_stm_segment(dev.idx()));
    auto [intensities, phases] = autd.link().drives(dev.idx(), Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x90; }));
  }

  auto g = autd3::gain::Uniform(0x81).with_phase(autd3::driver::Phase(0x91));
  ASSERT_TRUE(autd.send(g.with_segment(Segment::S1, true)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S1, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S1, autd.link().current_stm_segment(dev.idx()));
    auto [intensities, phases] = autd.link().drives(dev.idx(), Segment::S1, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x81; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x91; }));
  }

  ASSERT_TRUE(autd.send(autd3::gain::Uniform(0x82).with_phase(autd3::driver::Phase(0x92)).with_segment(Segment::S0, false)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S1, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S1, autd.link().current_stm_segment(dev.idx()));
    {
      auto [intensities, phases] = autd.link().drives(dev.idx(), Segment::S0, 0);
      ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x82; }));
      ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x92; }));
    }
    {
      auto [intensities, phases] = autd.link().drives(dev.idx(), Segment::S1, 0);
      ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x81; }));
      ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x91; }));
    }
  }

  ASSERT_TRUE(autd.send(autd3::driver::ChangeGainSegment(Segment::S0)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_stm_segment(dev.idx()));
  }
}
