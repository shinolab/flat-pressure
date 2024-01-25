#pragma once

#include <autd3.hpp>

template <typename L>
inline coro::task<void> tran_test(autd3::Controller<L>& autd) {
  auto silencer = autd3::ConfigureSilencer::default_();
  co_await autd.send_async(silencer);

  autd3::modulation::Sine m(150);  // 150Hz AM

  const autd3::gain::TransducerTest g([](const auto& dev, const auto& tr) -> std::optional<autd3::Drive> {
    if (dev.idx() == 0 && tr.idx() == 0) return autd3::Drive(autd3::Phase(0), autd3::EmitIntensity::maximum());
    if (dev.idx() == 0 && tr.idx() == 248) return autd3::Drive(autd3::Phase(0), autd3::EmitIntensity::maximum());
    return std::nullopt;
  });
  co_await autd.send_async(m, g);
}
