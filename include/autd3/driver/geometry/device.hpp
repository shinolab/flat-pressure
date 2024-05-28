#pragma once

#include <vector>

#include "autd3/def.hpp"
#include "autd3/driver/geometry/transducer.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver::geometry {

class Device {
  class DeviceView : public std::ranges::view_interface<DeviceView> {
   public:
    DeviceView() = default;
    explicit DeviceView(const std::vector<Transducer>& vec) : _begin(vec.cbegin()), _end(vec.cend()) {}

    [[nodiscard]] auto begin() const { return _begin; }
    [[nodiscard]] auto end() const { return _end; }

   private:
    std::vector<Transducer>::const_iterator _begin{}, _end{};
  };

 public:
  explicit Device(const size_t idx, const native_methods::DevicePtr ptr) : _idx(idx), _ptr(ptr) {
    const auto size = static_cast<size_t>(AUTDDeviceNumTransducers(_ptr));
    _transducers.clear();
    _transducers.reserve(size);
    for (uint32_t i = 0; i < size; i++) _transducers.emplace_back(i, _ptr);
  }

  ~Device() = default;
  Device(const Device& v) noexcept = default;  // LCOV_EXCL_LINE
  Device& operator=(const Device& obj) = default;
  Device(Device&& obj) = default;  // LCOV_EXCL_LINE
  Device& operator=(Device&& obj) = default;

  /**
   *@brief Device index
   *
   */
  [[nodiscard]] size_t idx() const { return _idx; }

  /**
   * @brief Get the number of transducers
   */
  [[nodiscard]] size_t num_transducers() const { return _transducers.size(); }

  /**
   * @brief Get center position of the transducers in the device
   */
  [[nodiscard]] Vector3 center() const {
    Vector3 v;
    AUTDDeviceCenter(_ptr, v.data());
    return v;
  }

  /**
   * @brief Speed of sound
   */
  [[nodiscard]] double sound_speed() const { return AUTDDeviceGetSoundSpeed(_ptr); }

  /**
   * @brief Set speed of sound
   */
  void set_sound_speed(const double value) const { AUTDDeviceSetSoundSpeed(_ptr, value); }

  /**
   * @brief Set the sound speed from temperature
   *
   * @param temp Temperature in celsius
   * @param k Ratio of specific heat
   * @param r Gas constant
   * @param m Molar mass
   */
  void set_sound_speed_from_temp(const double temp, const double k = 1.4, const double r = 8.31446261815324, const double m = 28.9647e-3) const {
    AUTDDeviceSetSoundSpeedFromTemp(_ptr, temp, k, r, m);
  }

  /**
   * @brief Attenuation coefficient
   */
  [[nodiscard]] double attenuation() const { return AUTDDeviceGetAttenuation(_ptr); }

  /**
   * @brief Set attenuation coefficient
   */
  void set_attenuation(const double value) const { AUTDDeviceSetAttenuation(_ptr, value); }

  /**
   * @brief get enable flag
   */
  [[nodiscard]] bool enable() const { return AUTDDeviceEnableGet(_ptr); }

  /**
   * @brief set enable flag
   */
  void set_enable(const bool value) const { AUTDDeviceEnableSet(_ptr, value); }

  void translate(Vector3 t) const { AUTDDeviceTranslate(_ptr, t.x(), t.y(), t.z()); }

  void rotate(Quaternion r) const { AUTDDeviceRotate(_ptr, r.w(), r.x(), r.y(), r.z()); }

  void affine(Vector3 t, Quaternion r) const { AUTDDeviceAffine(_ptr, t.x(), t.y(), t.z(), r.w(), r.x(), r.y(), r.z()); }

  [[nodiscard]] DeviceView transducers() const noexcept { return DeviceView(_transducers); }

  [[nodiscard]] std::vector<Transducer>::const_iterator cbegin() const noexcept { return _transducers.cbegin(); }
  [[nodiscard]] std::vector<Transducer>::const_iterator cend() const noexcept { return _transducers.cend(); }
  [[nodiscard]] std::vector<Transducer>::const_iterator begin() const noexcept { return _transducers.begin(); }
  [[nodiscard]] std::vector<Transducer>::const_iterator end() const noexcept { return _transducers.end(); }

  [[nodiscard]] const Transducer& operator[](const size_t i) const { return _transducers[i]; }

  [[nodiscard]] native_methods::DevicePtr ptr() const noexcept { return _ptr; }

 private:
  size_t _idx;
  native_methods::DevicePtr _ptr;
  std::vector<Transducer> _transducers{};
};

}  // namespace autd3::driver::geometry
