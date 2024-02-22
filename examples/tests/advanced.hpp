#pragma once

#include <unordered_map>
#include <vector>

#include "autd3.hpp"

class BurstModulation final : public autd3::Modulation<BurstModulation> {
 public:
  std::vector<autd3::EmitIntensity> calc() const override {
    std::vector<autd3::EmitIntensity> buffer(_buf_size, autd3::EmitIntensity::minimum());
    buffer[_buf_size - 1] = autd3::EmitIntensity::maximum();
    return buffer;
  }

  explicit BurstModulation(const size_t buf_size = 4000,
                           const autd3::SamplingConfiguration config = autd3::SamplingConfiguration::from_frequency(4e3)) noexcept
      : Modulation(config), _buf_size(buf_size) {}

 private:
  size_t _buf_size;
};

class MyUniformGain final : public autd3::Gain<MyUniformGain> {
 public:
  MyUniformGain() = default;

  AUTD3_API [[nodiscard]] std::unordered_map<size_t, std::vector<autd3::Drive>> calc(const autd3::Geometry& geometry) const override {
    return autd3::Gain<MyUniformGain>::transform(geometry, [this](const autd3::Device&, const autd3::Transducer&) {
      return autd3::Drive{autd3::Phase(0), autd3::EmitIntensity::maximum()};
    });
  }
};

template <typename L>
inline coro::task<void> advanced_test(autd3::Controller<L>& autd) {
  auto config = autd3::ConfigureSilencer::disable();
  co_await autd.send_async(config);

  MyUniformGain g;
  BurstModulation m;

  co_await autd.send_async(m, g);
}
