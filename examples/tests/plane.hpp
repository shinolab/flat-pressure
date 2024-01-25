#pragma once

#include "autd3.hpp"

template <typename L>
inline coro::task<void> plane_test(autd3::Controller<L>& autd) {
  auto silencer = autd3::ConfigureSilencer::default_();
  co_await autd.send_async(silencer);

  autd3::modulation::Sine m(150);  // 150Hz AM

  const autd3::Vector3 direction = autd3::Vector3::UnitZ();
  autd3::gain::Plane g(direction);

  co_await autd.send_async(m, g);
}
