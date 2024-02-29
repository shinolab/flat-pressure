#include <gtest/gtest.h>

#include <autd3/driver/datagram/reads_fpga_state.hpp>
#include <autd3/driver/datagram/silencer.hpp>
#include <autd3/driver/datagram/stm/gain.hpp>
#include <autd3/gain/focus.hpp>
#include <autd3/gain/uniform.hpp>
#include <ranges>

#include "utils.hpp"

using autd3::native_methods::Segment;

TEST(DriverDatagramSTM, GainSTM) {
  auto autd = autd3::controller::ControllerBuilder()
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .open(autd3::link::Audit::builder());

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureSilencer::disable()));

  autd3::driver::Vector3 center = autd.geometry().center() + autd3::driver::Vector3(0, 0, 150);
  auto stm = autd3::driver::GainSTM::from_freq(1).add_gains_from_iter(std::views::iota(0) | std::views::take(2) |
                                                                      std::views::transform([&](auto) { return autd3::gain::Focus(center); }));
  ASSERT_TRUE(autd.send(stm));
  for (const auto& dev : autd.geometry()) {
    ASSERT_TRUE(autd.link().is_stm_gain_mode(dev.idx(), autd3::native_methods::Segment::S0));
  }

  ASSERT_EQ(1, stm.frequency());
  ASSERT_EQ(2, stm.sampling_config().frequency());
  ASSERT_EQ(10240000u, stm.sampling_config().frequency_division());
  ASSERT_EQ(std::chrono::microseconds(2 * 500000), stm.period());
  ASSERT_EQ(std::chrono::microseconds(500000), stm.sampling_config().period());
  for (const auto& dev : autd.geometry()) {
    ASSERT_EQ(10240000u, autd.link().stm_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }

  stm = autd3::driver::GainSTM::from_sampling_config(autd3::driver::SamplingConfiguration::from_frequency_division(512))
            .add_gain(autd3::gain::Uniform(0x80))
            .add_gain(autd3::gain::Uniform(0x80))
            .with_mode(autd3::native_methods::GainSTMMode::PhaseIntensityFull);
  ASSERT_TRUE(autd.send(stm));
  ASSERT_EQ(20000.0, stm.frequency());
  ASSERT_EQ(2 * 20000.0, stm.sampling_config().frequency());
  ASSERT_EQ(512u, stm.sampling_config().frequency_division());
  ASSERT_EQ(std::chrono::microseconds(25), stm.sampling_config().period());
  for (const auto& dev : autd.geometry()) {
    ASSERT_EQ(512u, autd.link().stm_frequency_division(dev.idx(), autd3::native_methods::Segment::S0));
  }

  for (const auto& dev : autd.geometry()) {
    ASSERT_EQ(2u, autd.link().stm_cycle(dev.idx(), autd3::native_methods::Segment::S0));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));

    std::tie(intensities, phases) = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 1);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  stm.with_mode(autd3::native_methods::GainSTMMode::PhaseFull);
  ASSERT_TRUE(autd.send(stm));
  for (const auto& dev : autd.geometry()) {
    ASSERT_EQ(2u, autd.link().stm_cycle(dev.idx(), autd3::native_methods::Segment::S0));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0xFF; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));

    std::tie(intensities, phases) = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 1);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0xFF; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }

  stm.with_mode(autd3::native_methods::GainSTMMode::PhaseHalf);
  ASSERT_TRUE(autd.send(stm));
  for (const auto& dev : autd.geometry()) {
    ASSERT_EQ(2u, autd.link().stm_cycle(dev.idx(), autd3::native_methods::Segment::S0));
    auto [intensities, phases] = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 0);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0xFF; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));

    std::tie(intensities, phases) = autd.link().drives(dev.idx(), autd3::native_methods::Segment::S0, 1);
    ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0xFF; }));
    ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0; }));
  }
}

TEST(DriverDatagramSTM, GainSTMSegment) {
  auto autd = create_controller();

  ASSERT_TRUE(autd.send(autd3::driver::ConfigureReadsFPGAState([](const auto&) { return true; })));

  auto infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_stm_segment(dev.idx()));
  }

  autd3::driver::Vector3 center = autd.geometry().center() + autd3::driver::Vector3(0, 0, 150);
  auto stm = autd3::driver::GainSTM::from_freq(1).add_gains_from_iter(std::views::iota(0) | std::views::take(2) |
                                                                      std::views::transform([&](auto) { return autd3::gain::Focus(center); }));

  ASSERT_TRUE(autd.send(stm));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_stm_segment(dev.idx()));
  }

  ASSERT_TRUE(autd.send(stm.with_segment(Segment::S1, true)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S1, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S1, autd.link().current_stm_segment(dev.idx()));
  }

  ASSERT_TRUE(autd.send(
      autd3::driver::GainSTM::from_freq(1)
          .add_gains_from_iter(std::views::iota(0) | std::views::take(2) | std::views::transform([&](auto) { return autd3::gain::Focus(center); }))
          .with_segment(Segment::S0, false)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S1, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S1, autd.link().current_stm_segment(dev.idx()));
  }

  ASSERT_TRUE(autd.send(autd3::driver::ChangeGainSTMSegment(Segment::S0)));
  infos = autd.fpga_info();
  for (auto& dev : autd.geometry()) {
    ASSERT_EQ(std::nullopt, infos[dev.idx()].value().current_gain_segment());
    ASSERT_EQ(Segment::S0, infos[dev.idx()].value().current_stm_segment());
    ASSERT_EQ(Segment::S0, autd.link().current_stm_segment(dev.idx()));
  }
}
