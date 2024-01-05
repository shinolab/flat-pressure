// File: mod_audio_file.hpp
// Project: tests
// Created Date: 16/05/2022
// Author: Shun Suzuki
// -----
// Last Modified: 05/01/2024
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022-2023 Shun Suzuki. All rights reserved.
//

#pragma once

#include <filesystem>

#include "autd3.hpp"
#include "autd3/modulation/audio_file.hpp"

namespace fs = std::filesystem;

template <typename L>
inline coro::task<void> mod_audio_file_test(autd3::Controller<L>& autd) {
  auto silencer = autd3::ConfigureSilencer::default_();
  co_await autd.send_async(silencer);

  const fs::path path = fs::path(AUTD3_RESOURCE_PATH).append("sin150.wav");
  autd3::modulation::audio_file::Wav m(path);

  const autd3::Vector3 center = autd.geometry().center() + autd3::Vector3(0.0, 0.0, 150.0);

  autd3::gain::Focus g(center);

  co_await autd.send_async(m, g);
}
