#pragma once

#include <chrono>
#include <string>
#include <variant>

#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/driver/link.hpp"
#include "autd3/exception.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/autd3capi-link-visualizer.h"
#include "autd3/native_methods/utils.hpp"

namespace autd3::link {

using native_methods::CMap;

struct PlottersBackend {
  static constexpr native_methods::Backend backend = native_methods::Backend::Plotters;
};
struct PythonBackend {
  static constexpr native_methods::Backend backend = native_methods::Backend::Python;
};
struct NullBackend {
  static constexpr native_methods::Backend backend = native_methods::Backend::Null;
};

struct Sphere {
  static constexpr native_methods::Directivity directivity = native_methods::Directivity::Sphere;
};
struct T4010A1 {
  static constexpr native_methods::Directivity directivity = native_methods::Directivity::T4010A1;
};

class Visualizer;

struct PlotRange {
  friend class Visualizer;

  double x_start;
  double x_end;
  double y_start;
  double y_end;
  double z_start;
  double z_end;
  double resolution;

  PlotRange() = default;
  explicit PlotRange(const double x_start, const double x_end, const double y_start, const double y_end, const double z_start, const double z_end,
                     const double resolution = 1)
      : x_start(x_start), x_end(x_end), y_start(y_start), y_end(y_end), z_start(z_start), z_end(z_end), resolution(resolution) {}

  std::vector<driver::Vector3> observe_points() {
    const auto range = ptr();
    const auto len = AUTDLinkVisualizerPlotRangeObservePointsLen(range);
    std::vector<driver::Vector3> points;
    points.resize(len);
    AUTDLinkVisualizerPlotRangeObservePoints(range, reinterpret_cast<double*>(points.data()));
    return points;
  }

 private:
  [[nodiscard]] native_methods::PlotRangePtr ptr() const {
    return native_methods::AUTDLinkVisualizerPlotRange(x_start, x_end, y_start, y_end, z_start, z_end, resolution);
  }
};

struct PlotConfig {
  std::pair<uint32_t, uint32_t> figsize = std::make_pair(960, 640);
  double cbar_size = 0.15;
  uint32_t font_size = 24;
  uint32_t label_area_size = 80;
  uint32_t margin = 10;
  double ticks_step = 10;
  CMap cmap = CMap::Jet;
  std::string fname;
};

struct PyPlotConfig {
  std::pair<int32_t, int32_t> figsize = std::make_pair(8, 6);
  int32_t dpi = 72;
  std::string cbar_position = "right";
  std::string cbar_size = "5%";
  std::string cbar_pad = "3%";
  int32_t fontsize = 12;
  double ticks_step = 10;
  std::string cmap = "jet";
  bool show = false;
  std::string fname;
};

struct NullPlotConfig {};

using Config = std::variant<PlotConfig, PyPlotConfig, NullPlotConfig>;

/**
 * @brief Link for visualizing
 *
 */
class Visualizer final {
  native_methods::LinkPtr _ptr;
  native_methods::Backend _backend;
  native_methods::Directivity _directivity;

  [[nodiscard]] native_methods::ConfigPtr get_plot_config(Config config) const {
    if (_backend == native_methods::Backend::Plotters && std::holds_alternative<PlotConfig>(config)) {
      const auto& [figsize, cbar_size, font_size, label_area_size, margin, ticks_step, cmap, fname] = std::get<PlotConfig>(config);
      auto ptr = validate(native_methods::AUTDLinkVisualizerPlotConfig(figsize.first, figsize.second, cbar_size, font_size, label_area_size, margin,
                                                                       ticks_step, cmap, fname.c_str()));
      return native_methods::ConfigPtr{ptr._0};
    }

    if (_backend == native_methods::Backend::Python && std::holds_alternative<PyPlotConfig>(config)) {
      const auto& [figsize, dpi, cbar_position, cbar_size, cbar_pad, fontsize, ticks_step, cmap, show, fname] = std::get<PyPlotConfig>(config);
      auto ptr = validate(native_methods::AUTDLinkVisualizerPyPlotConfig(figsize.first, figsize.second, dpi, cbar_position.c_str(), cbar_size.c_str(),
                                                                         cbar_pad.c_str(), fontsize, ticks_step, cmap.c_str(), show, fname.c_str()));
      return native_methods::ConfigPtr{ptr._0};
    }

    if (_backend == native_methods::Backend::Null && std::holds_alternative<NullPlotConfig>(config))
      return native_methods::ConfigPtr{native_methods::AUTDLinkVisualizerNullPlotConfigDefault()._0};

    throw AUTDException("Invalid plot config type.");
  }

 public:
  class Builder final {
    friend class Visualizer;
    friend class controller::ControllerBuilder;

    std::optional<int32_t> _gpu_idx;
    native_methods::Backend _backend;
    native_methods::Directivity _directivity;

    explicit Builder(const native_methods::Backend backend = native_methods::Backend::Plotters,
                     const native_methods::Directivity directivity = native_methods::Directivity::Sphere)
        : _backend(backend), _directivity(directivity) {}

    [[nodiscard]] Visualizer resolve_link(const native_methods::LinkPtr link) const { return Visualizer{link, _backend, _directivity}; }

   public:
    using Link = Visualizer;

    [[nodiscard]] native_methods::LinkBuilderPtr ptr() const {
      switch (_backend) {
        case native_methods::Backend::Plotters:
          switch (_directivity) {
            case native_methods::Directivity::Sphere:
              return native_methods::AUTDLinkVisualizerSpherePlotters(_gpu_idx.has_value(), _gpu_idx.value_or(0));
            case native_methods::Directivity::T4010A1:
              return native_methods::AUTDLinkVisualizerT4010A1Plotters(_gpu_idx.has_value(), _gpu_idx.value_or(0));
            default:                                    // LCOV_EXCL_LINE
              throw std::runtime_error("unreachable");  // LCOV_EXCL_LINE
          }
        case native_methods::Backend::Python:
          switch (_directivity) {
            case native_methods::Directivity::Sphere:
              return native_methods::AUTDLinkVisualizerSpherePython(_gpu_idx.has_value(), _gpu_idx.value_or(0));
            case native_methods::Directivity::T4010A1:
              return native_methods::AUTDLinkVisualizerT4010A1Python(_gpu_idx.has_value(), _gpu_idx.value_or(0));
            default:                                    // LCOV_EXCL_LINE
              throw std::runtime_error("unreachable");  // LCOV_EXCL_LINE
          }
        case native_methods::Backend::Null:
          switch (_directivity) {
            case native_methods::Directivity::Sphere:
              return native_methods::AUTDLinkVisualizerSphereNull(_gpu_idx.has_value(), _gpu_idx.value_or(0));
            case native_methods::Directivity::T4010A1:
              return native_methods::AUTDLinkVisualizerT4010A1Null(_gpu_idx.has_value(), _gpu_idx.value_or(0));
            default:                                    // LCOV_EXCL_LINE
              throw std::runtime_error("unreachable");  // LCOV_EXCL_LINE
          }
        default:                                    // LCOV_EXCL_LINE
          throw std::runtime_error("unreachable");  // LCOV_EXCL_LINE
      }
    }

    [[nodiscard]] Builder with_gpu(const int32_t gpu_idx) {  // LCOV_EXCL_START
      _gpu_idx = gpu_idx;
      return *this;
    }  // LCOV_EXCL_STOP

    template <typename B>
    [[nodiscard]] Builder with_backend() {
      _backend = B::backend;
      return *this;
    }

    template <typename D>
    [[nodiscard]] Builder with_directivity() {
      _directivity = D::directivity;
      return *this;
    }
  };

  static Builder builder() { return Builder(); }
  static Builder plotters() { return Builder().with_backend<PlottersBackend>(); }
  static Builder python() { return Builder().with_backend<PythonBackend>(); }
  static Builder null() { return Builder().with_backend<NullBackend>(); }

  Visualizer() = delete;

  explicit Visualizer(const native_methods::LinkPtr ptr, native_methods::Backend backend, native_methods::Directivity directivity)
      : _ptr(ptr), _backend(backend), _directivity(directivity) {}

  [[nodiscard]] std::vector<uint8_t> phases_of(const size_t idx) const {
    const auto size = AUTDLinkVisualizerPhasesOf(_ptr, _backend, _directivity, static_cast<uint32_t>(idx), nullptr);
    std::vector<uint8_t> buf;
    buf.resize(size);
    AUTDLinkVisualizerPhasesOf(_ptr, _backend, _directivity, static_cast<uint32_t>(idx), buf.data());
    return buf;
  }  // LCOV_EXCL_LINE

  [[nodiscard]] std::vector<uint8_t> phases() const { return phases_of(0); }

  [[nodiscard]] std::vector<uint8_t> intensities_of(const size_t idx) const {
    const auto size = AUTDLinkVisualizerIntensitiesOf(_ptr, _backend, _directivity, static_cast<uint32_t>(idx), nullptr);
    std::vector<uint8_t> buf;
    buf.resize(size);
    AUTDLinkVisualizerIntensitiesOf(_ptr, _backend, _directivity, static_cast<uint32_t>(idx), buf.data());
    return buf;
  }  // LCOV_EXCL_LINE

  [[nodiscard]] std::vector<uint8_t> intensities() const { return intensities_of(0); }

  [[nodiscard]] std::vector<uint8_t> modulation() const {
    const auto size = AUTDLinkVisualizerModulation(_ptr, _backend, _directivity, nullptr);
    std::vector<uint8_t> buf;
    buf.resize(size);
    AUTDLinkVisualizerModulation(_ptr, _backend, _directivity, buf.data());
    return buf;
  }  // LCOV_EXCL_LINE

  [[nodiscard]] std::vector<std::complex<double>> calc_field_of(std::vector<driver::Vector3>& points, const driver::geometry::Geometry& geometry,
                                                                const size_t idx) const {
    const auto points_len = static_cast<uint32_t>(points.size());
    const auto points_ptr = reinterpret_cast<double*>(points.data());
    std::vector<std::complex<double>> buf;
    buf.resize(points_len);
    native_methods::validate(AUTDLinkVisualizerCalcFieldOf(_ptr, _backend, _directivity, points_ptr, points_len, geometry.ptr(),
                                                           static_cast<uint32_t>(idx), reinterpret_cast<double*>(buf.data())));
    return buf;
  }  // LCOV_EXCL_LINE

  [[nodiscard]] std::vector<std::complex<double>> calc_field(std::vector<driver::Vector3>& points, const driver::geometry::Geometry& geometry) const {
    return calc_field_of(points, geometry, 0);
  }

  void plot_field_of(const Config& config, const PlotRange& range, const driver::geometry::Geometry& geometry, const size_t idx) const {
    const auto config_ptr = get_plot_config(config);
    native_methods::validate(
        AUTDLinkVisualizerPlotFieldOf(_ptr, _backend, _directivity, config_ptr, range.ptr(), geometry.ptr(), static_cast<uint32_t>(idx)));
  }

  void plot_field(const Config& config, const PlotRange& range, const driver::geometry::Geometry& geometry) const {
    plot_field_of(config, range, geometry, 0);
  }

  void plot_phase_of(const Config& config, const driver::geometry::Geometry& geometry, const size_t idx) const {
    const auto config_ptr = get_plot_config(config);
    native_methods::validate(AUTDLinkVisualizerPlotPhaseOf(_ptr, _backend, _directivity, config_ptr, geometry.ptr(), static_cast<uint32_t>(idx)));
  }

  void plot_phase(const Config& config, const driver::geometry::Geometry& geometry) const { plot_phase_of(config, geometry, 0); }

  void plot_modulation(const Config& config) const {
    const auto config_ptr = get_plot_config(config);
    native_methods::validate(AUTDLinkVisualizerPlotModulation(_ptr, _backend, _directivity, config_ptr));
  }
};

}  // namespace autd3::link
