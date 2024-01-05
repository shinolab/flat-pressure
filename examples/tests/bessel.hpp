// File: bessel.hpp
// Project: tests
// Created Date: 11/05/2022
// Author: Shun Suzuki
// -----
// Last Modified: 05/01/2024
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022-2023 Shun Suzuki. All rights reserved.
//

#pragma once

#include <autd3.hpp>

template <typename L>
inline coro::task<void> bessel_test(autd3::Controller<L>& autd) {
  auto silencer = autd3::ConfigureSilencer::default_();
  co_await autd.send_async(silencer);

  autd3::modulation::Sine m(150);  // 150Hz AM

  const autd3::Vector3 apex = autd.geometry().center();
  autd3::gain::Bessel g(apex, autd3::Vector3::UnitZ(), 13.0 / 180.0 * autd3::pi);

  co_await autd.send_async(m, g);
}
