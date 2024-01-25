#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/controller/controller.hpp>
#include <autd3/driver/autd3_device.hpp>
#include <autd3/gain/uniform.hpp>
#include <autd3/link/visualizer.hpp>
#include <autd3/modulation/static.hpp>

void visualizer_test_with(autd3::controller::Controller<autd3::link::Visualizer>& autd, autd3::link::Config config) {
  autd3::driver::Vector3 center = autd.geometry().center() + autd3::driver::Vector3(0, 0, 150);

  auto g = autd3::gain::Uniform(0x80).with_phase(autd3::driver::Phase(0x81));
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

  std::vector<autd3::driver::Vector3> points{center};
  (void)autd.link().calc_field(points, autd.geometry());

  autd.close();
}

TEST(Link, PlotRange) {
  auto plot_range = autd3::link::PlotRange();
  plot_range.x_start = -20;
  plot_range.x_end = 20;
  plot_range.y_start = -30;
  plot_range.y_end = 30;
  plot_range.z_start = 0;
  plot_range.z_end = 0;
  plot_range.resolution = 1;

  const auto points = plot_range.observe_points();
  ASSERT_EQ(points.size(), 41 * 61);
  ASSERT_EQ(points[0], autd3::driver::Vector3(-20, -30, 0));
  ASSERT_EQ(points[1], autd3::driver::Vector3(-19, -30, 0));
  ASSERT_EQ(points[points.size() - 1], autd3::driver::Vector3(20, 30, 0));
}

TEST(Link, Visualizer_Plotters) {
  {
    auto autd =
        autd3::controller::ControllerBuilder()
            .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
            .open_with(autd3::link::Visualizer::builder().with_backend<autd3::link::PlottersBackend>().with_directivity<autd3::link::Sphere>());
    visualizer_test_with(autd, autd3::link::PlotConfig());
  }
  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open_with(autd3::link::Visualizer::plotters().with_directivity<autd3::link::T4010A1>());
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
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open_with(autd3::link::Visualizer::builder().with_backend<autd3::link::PythonBackend>().with_directivity<autd3::link::Sphere>());
    visualizer_test_with(autd, autd3::link::PyPlotConfig());
  }
  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open_with(autd3::link::Visualizer::python().with_directivity<autd3::link::T4010A1>());
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
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open_with(autd3::link::Visualizer::builder().with_backend<autd3::link::NullBackend>().with_directivity<autd3::link::Sphere>());
    visualizer_test_with(autd, autd3::link::NullPlotConfig());
  }
  {
    auto autd = autd3::controller::ControllerBuilder()
                    .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                    .open_with(autd3::link::Visualizer::null().with_directivity<autd3::link::T4010A1>());
    visualizer_test_with(autd, autd3::link::NullPlotConfig());
  }
}

TEST(Link, VisualizerInvalidConfig) {
  auto autd = autd3::controller::ControllerBuilder()
                  .add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero()))
                  .open_with(autd3::link::Visualizer::plotters());
  ASSERT_THROW(autd.link().plot_modulation(autd3::link::NullPlotConfig()), autd3::AUTDException);
}
