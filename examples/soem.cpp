// File: soem.cpp
// Project: examples
// Created Date: 16/05/2022
// Author: Shun Suzuki
// -----
// Last Modified: 05/01/2024
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022-2023 Shun Suzuki. All rights reserved.
//

#include "autd3/link/soem.hpp"

#include <iostream>

#include "autd3.hpp"
#include "runner.hpp"
#include "util.hpp"

[[noreturn]] void on_lost(const char* msg) {
  std::cerr << "Link is lost\n";
  std::cerr << msg;
#ifdef __APPLE__
  // mac does not have quick_exit??
  exit(-1);
#else
  std::quick_exit(-1);
#endif
}

coro::task<int> main_() {
  auto autd = co_await autd3::ControllerBuilder()
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .open_with_async(autd3::link::SOEM::builder().with_on_lost(&on_lost));
  auto res = co_await run(autd);
  co_return res;
}

int main() try { return sync_wait(main_()); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
