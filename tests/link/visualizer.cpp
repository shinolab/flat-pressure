// File: visualizer.cpp
// Project: link
// Created Date: 11/12/2023
// Author: Shun Suzuki
// -----
// Last Modified: 11/12/2023
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2023 Shun Suzuki. All rights reserved.
//

#include <gtest/gtest.h>

#include <autd3/gain/uniform.hpp>
#include <autd3/internal/controller.hpp>
#include <autd3/link/visualizer.hpp>
#include <autd3/modulation/static.hpp>

void visualizer_test_with(autd3::internal::Controller<autd3::link::Visualizer>& autd, autd3::link::Config config) {
  autd3::internal::Vector3 center = autd.geometry().center() + autd3::internal::Vector3(0, 0, 150);

  auto g = autd3::gain::Uniform(0x80).with_phase(autd3::internal::Phase(0x81));
  auto m = autd3::modulation::Static().with_intensity(0x82);

  autd.send(m, g);

  autd.link().plot_phase(config, autd.geometry());
  autd.link().plot_field(config, autd3::link::PlotRange(center[0] - 50, center[0] + 50, center[1], center[1], center[2], center[2], 1),
                         autd.geometry());

  autd.link().plot_modulation(config);

  auto intensities = autd.link().intensities();
  ASSERT_TRUE(std::ranges::all_of(intensities, [](auto d) { return d == 0x80; }));
  auto phases = autd.link().phases();
  ASSERT_TRUE(std::ranges::all_of(phases, [](auto p) { return p == 0x81; }));
  auto modulation = autd.link().modulation();
  ASSERT_EQ(modulation.size(), 2);
  ASSERT_TRUE(std::ranges::all_of(modulation, [](auto m) { return m == 0x82; }));

  std::vector<autd3::internal::Vector3> points{center};
  (void)autd.link().calc_field(points, autd.geometry());

  autd.close();
}

TEST(Link, Visualizer_Plotters) {
  {
    auto autd =
        autd3::internal::ControllerBuilder()
            .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
            .open_with_async(autd3::link::Visualizer::builder().with_backend<autd3::link::PlottersBackend>().with_directivity<autd3::link::Sphere>())
            .get();
    visualizer_test_with(autd, autd3::link::PlotConfig());
  }
  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with_async(autd3::link::Visualizer::plotters().with_directivity<autd3::link::T4010A1>())
                    .get();
    auto config = autd3::link::PlotConfig();
    config.fname = "test.png";
    config.figsize = std::make_pair(960, 640);
    config.cbar_size = 0.15;
    config.font_size = 24;
    config.label_area_size = 80;
    config.margin = 10;
    config.ticks_step = 10.0;
    config.cmap = autd3::link::CMap::Jet;
    visualizer_test_with(autd, config);
  }
}

TEST(Link, Visualizer_Python) {
  {
    auto autd =
        autd3::internal::ControllerBuilder()
            .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
            .open_with_async(autd3::link::Visualizer::builder().with_backend<autd3::link::PythonBackend>().with_directivity<autd3::link::Sphere>())
            .get();
    visualizer_test_with(autd, autd3::link::PyPlotConfig());
  }
  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with_async(autd3::link::Visualizer::python().with_directivity<autd3::link::T4010A1>())
                    .get();
    auto config = autd3::link::PyPlotConfig();
    config.fname = "test.png";
    config.figsize = std::make_pair(8, 6);
    config.dpi = 72;
    config.cbar_position = "right";
    config.cbar_size = "5%";
    config.cbar_pad = "3%";
    config.fontsize = 12;
    config.ticks_step = 10.0;
    config.cmap = "jet";
    config.show = false;
    visualizer_test_with(autd, config);
  }
}

TEST(Link, Visualizer_Null) {
  {
    auto autd =
        autd3::internal::ControllerBuilder()
            .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
            .open_with_async(autd3::link::Visualizer::builder().with_backend<autd3::link::NullBackend>().with_directivity<autd3::link::Sphere>())
            .get();
    visualizer_test_with(autd, autd3::link::NullPlotConfig());
  }
  {
    auto autd = autd3::internal::ControllerBuilder()
                    .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                    .open_with_async(autd3::link::Visualizer::null().with_directivity<autd3::link::T4010A1>())
                    .get();
    visualizer_test_with(autd, autd3::link::NullPlotConfig());
  }
}

TEST(Link, VisualizerInvalidConfig) {
  auto autd = autd3::internal::ControllerBuilder()
                  .add_device(autd3::internal::geometry::AUTD3(autd3::internal::Vector3::Zero()))
                  .open_with_async(autd3::link::Visualizer::plotters())
                  .get();
  ASSERT_THROW(autd.link().plot_modulation(autd3::link::NullPlotConfig()), autd3::internal::AUTDException);
}
