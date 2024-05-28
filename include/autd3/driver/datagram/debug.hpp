#pragma once

#include <concepts>
#include <variant>

#include "autd3/driver/geometry/device.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/driver/geometry/transducer.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::driver {

struct InternalDebugTypes {
  struct none_t {};
  struct base_signal_t {};
  struct thermo_t {};
  struct force_fan_t {};
  struct sync_t {};
  struct mod_segment_t {};
  struct mod_idx_t {
    uint16_t value;
  };
  struct stm_segment_t {};
  struct stm_idx_t {
    uint16_t value;
  };
  struct is_stm_mode_t {};
  struct pwm_out_t {
    const geometry::Transducer* value;
  };
  struct direct_t {
    bool value;
  };

  struct TypeVisitor {
    uint8_t operator()(none_t) { return 0x00; }
    uint8_t operator()(base_signal_t) { return 0x01; }
    uint8_t operator()(thermo_t) { return 0x02; }
    uint8_t operator()(force_fan_t) { return 0x03; }
    uint8_t operator()(sync_t) { return 0x10; }
    uint8_t operator()(mod_segment_t) { return 0x20; }
    uint8_t operator()(mod_idx_t) { return 0x21; }
    uint8_t operator()(stm_segment_t) { return 0x50; }
    uint8_t operator()(stm_idx_t) { return 0x51; }
    uint8_t operator()(is_stm_mode_t) { return 0x52; }
    uint8_t operator()(pwm_out_t) { return 0xE0; }
    uint8_t operator()(direct_t) { return 0xF0; }
  };

  struct ValueVisitor {
    uint16_t operator()(none_t) { return 0; }
    uint16_t operator()(base_signal_t) { return 0; }
    uint16_t operator()(thermo_t) { return 0; }
    uint16_t operator()(force_fan_t) { return 0; }
    uint16_t operator()(sync_t) { return 0; }
    uint16_t operator()(mod_segment_t) { return 0; }
    uint16_t operator()(mod_idx_t idx) { return idx.value; }
    uint16_t operator()(stm_segment_t) { return 0; }
    uint16_t operator()(stm_idx_t idx) { return idx.value; }
    uint16_t operator()(is_stm_mode_t) { return 0; }
    uint16_t operator()(pwm_out_t tr) { return static_cast<uint16_t>(tr.value->idx()); }
    uint16_t operator()(direct_t v) { return static_cast<uint16_t>(v.value); }
  };
};

struct DebugType {
  static constexpr InternalDebugTypes::none_t None{};
  static constexpr InternalDebugTypes::base_signal_t BaseSignal{};
  static constexpr InternalDebugTypes::thermo_t Thermo{};
  static constexpr InternalDebugTypes::force_fan_t ForceFan{};
  static constexpr InternalDebugTypes::sync_t Sync{};
  static constexpr InternalDebugTypes::mod_segment_t ModSegment{};
  static constexpr InternalDebugTypes::mod_idx_t ModIdx(uint16_t idx) { return {idx}; }
  static constexpr InternalDebugTypes::stm_segment_t StmSegment{};
  static constexpr InternalDebugTypes::stm_idx_t StmIdx(uint16_t idx) { return {idx}; }
  static constexpr InternalDebugTypes::is_stm_mode_t IsStmMode{};
  static constexpr InternalDebugTypes::pwm_out_t PwmOut(const geometry::Transducer* tr) { return {tr}; }
  static constexpr InternalDebugTypes::direct_t Direct(bool v) { return {v}; }
};

using DebugTypes =
    std::variant<InternalDebugTypes::none_t, InternalDebugTypes::base_signal_t, InternalDebugTypes::thermo_t, InternalDebugTypes::force_fan_t,
                 InternalDebugTypes::sync_t, InternalDebugTypes::mod_segment_t, InternalDebugTypes::mod_idx_t, InternalDebugTypes::stm_segment_t,
                 InternalDebugTypes::stm_idx_t, InternalDebugTypes::is_stm_mode_t, InternalDebugTypes::pwm_out_t, InternalDebugTypes::direct_t>;

template <class F>
concept configure_debug_settings_f = requires(F f, const geometry::Device& d) {
  { f(d) } -> std::same_as<std::array<DebugTypes, 4>>;
};

/**
 * @brief Datagram to configure debug output
 */
template <configure_debug_settings_f F>
class ConfigureDebugSettings final {
  using native_f = native_methods::DebugSettings (*)(const void*, native_methods::GeometryPtr, uint32_t);

 public:
  explicit ConfigureDebugSettings(F f) : _f(std::move(f)) {
    _f_native = +[](const void* context, const native_methods::GeometryPtr geometry_ptr, const uint32_t dev_idx) -> native_methods::DebugSettings {
      const geometry::Device dev(dev_idx, AUTDDevice(geometry_ptr, dev_idx));
      const auto v = static_cast<const ConfigureDebugSettings*>(context)->_f(dev);
      native_methods::DebugSettings res{};
      res.ty[0] = std::visit(InternalDebugTypes::TypeVisitor{}, v[0]);
      res.ty[1] = std::visit(InternalDebugTypes::TypeVisitor{}, v[1]);
      res.ty[2] = std::visit(InternalDebugTypes::TypeVisitor{}, v[2]);
      res.ty[3] = std::visit(InternalDebugTypes::TypeVisitor{}, v[3]);
      res.value[0] = std::visit(InternalDebugTypes::ValueVisitor{}, v[0]);
      res.value[1] = std::visit(InternalDebugTypes::ValueVisitor{}, v[1]);
      res.value[2] = std::visit(InternalDebugTypes::ValueVisitor{}, v[2]);
      res.value[3] = std::visit(InternalDebugTypes::ValueVisitor{}, v[3]);
      return res;
    };
  }

  [[nodiscard]] native_methods::DatagramPtr ptr(const geometry::Geometry& geometry) const {
    return native_methods::AUTDDatagramConfigureDebugSettings(const_cast<void*>(reinterpret_cast<const void*>(_f_native)),
                                                              const_cast<void*>(static_cast<const void*>(this)), geometry.ptr());
  }

 private:
  F _f;
  native_f _f_native;
};

}  // namespace autd3::driver
