#pragma once

#include <algorithm>
#include <numeric>
#include <ranges>
#include <vector>

#include "autd3/def.hpp"
#include "autd3/driver/geometry/device.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver::geometry {

class Geometry {
  class GeometryView : public std::ranges::view_interface<GeometryView> {
   public:
    GeometryView() = default;
    explicit GeometryView(const std::vector<Device>& vec) : _begin(vec.cbegin()), _end(vec.cend()) {}

    [[nodiscard]] auto begin() const { return _begin; }
    [[nodiscard]] auto end() const { return _end; }

   private:
    std::vector<Device>::const_iterator _begin{}, _end{};
  };

 public:
  explicit Geometry(const native_methods::GeometryPtr ptr) : _ptr(ptr) {
    const auto size = AUTDGeometryNumDevices(_ptr);
    _devices.clear();
    _devices.reserve(size);
    for (uint32_t i = 0; i < size; i++) _devices.emplace_back(static_cast<size_t>(i), AUTDDevice(_ptr, i));
  }

  ~Geometry() = default;
  Geometry(const Geometry& v) noexcept = default;
  Geometry& operator=(const Geometry& obj) = default;
  Geometry(Geometry&& obj) = default;
  Geometry& operator=(Geometry&& obj) = default;

  /**
   * @brief Get the number of devices
   */
  [[nodiscard]] size_t num_devices() const { return _devices.size(); }

  /**
   * @brief Get the number of transducers
   */
  [[nodiscard]] size_t num_transducers() const {
    return std::accumulate(_devices.begin(), _devices.end(), size_t{0}, [](const size_t acc, const Device& d) { return acc + d.num_transducers(); });
  }

  /**
   * @brief Get center position of all devices
   */
  [[nodiscard]] Vector3 center() const {
    return std::accumulate(_devices.begin(), _devices.end(), Vector3(0, 0, 0),
                           [](const Vector3& acc, const Device& d) -> Vector3 {
                             Vector3 res = acc + d.center();
                             return res;
                           }) /
           static_cast<double>(num_devices());
  }

  /*
   * @brief Enumerate enabled devices
   */
  [[nodiscard]] auto devices() const noexcept {
    return GeometryView(_devices) | std::views::filter([](const auto& dev) { return dev.enable(); });
  }

  /**
   * @brief Set speed of sound of enabled devices
   */
  void set_sound_speed(const double value) const {
    std::ranges::for_each(devices(), [value](const auto& dev) { dev.set_sound_speed(value); });
  }

  /**
   * @brief Set the sound speed from temperature of enabled devices
   *
   * @param temp Temperature in celsius
   * @param k Ratio of specific heat
   * @param r Gas constant
   * @param m Molar mass
   */
  void set_sound_speed_from_temp(const double temp, const double k = 1.4, const double r = 8.31446261815324, const double m = 28.9647e-3) const {
    std::ranges::for_each(devices(), [temp, k, r, m](const auto& dev) { dev.set_sound_speed_from_temp(temp, k, r, m); });
  }

  [[nodiscard]] std::vector<Device>::iterator begin() noexcept { return _devices.begin(); }
  [[nodiscard]] std::vector<Device>::iterator end() noexcept { return _devices.end(); }
  [[nodiscard]] std::vector<Device>::const_iterator cbegin() const noexcept { return _devices.cbegin(); }
  [[nodiscard]] std::vector<Device>::const_iterator cend() const noexcept { return _devices.cend(); }
  [[nodiscard]] const Device& operator[](const size_t i) const { return _devices[i]; }
  [[nodiscard]] Device& operator[](const size_t i) { return _devices[i]; }
  [[nodiscard]] native_methods::GeometryPtr ptr() const noexcept { return _ptr; }
  // LCOV_EXCL_START
 private:
  native_methods::GeometryPtr _ptr;
  std::vector<Device> _devices{};
  // LCOV_EXCL_STOP
};

}  // namespace autd3::driver::geometry
