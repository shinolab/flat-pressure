// File: bessel.hpp
// Project: tests
// Created Date: 11/05/2022
// Author: Shun Suzuki
// -----
// Last Modified: 13/11/2023
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022-2023 Shun Suzuki. All rights reserved.
//

#pragma once

#include <autd3.hpp>

template <typename L>
inline void bessel_test(autd3::Controller<L>& autd) {
  autd3::Silencer silencer;
  autd.send_async(silencer).get();

  autd3::modulation::Sine m(150);  // 150Hz AM

  const autd3::Vector3 apex = autd.geometry().center();
  autd3::gain::Bessel g(apex, autd3::Vector3::UnitZ(), 13.0 / 180.0 * autd3::pi);

  autd.send_async(m, g).get();
}