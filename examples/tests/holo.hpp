#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "autd3.hpp"
#include "autd3/gain/holo.hpp"

template <typename L>
inline coro::task<void> holo_test(autd3::Controller<L>& autd) {
  auto silencer = autd3::ConfigureSilencer::default_();
  co_await autd.send_async(silencer);

  autd3::modulation::Sine m(150);  // 150Hz AM

  const autd3::Vector3 center = autd.geometry().center() + autd3::Vector3(0.0, 0.0, 150.0);

  std::cout << "Select Optimization Method (default is GSPAT)" << std::endl;

  std::cout << "[0]: SDP" << std::endl;
  std::cout << "[1]: GS" << std::endl;
  std::cout << "[2]: GSPAT" << std::endl;
  std::cout << "[3]: Naive" << std::endl;
  std::cout << "[4]: LM" << std::endl;
  std::cout << "[5]: Greedy" << std::endl;
  std::cout << "[Others]: GS-PAT" << std::endl;
  std::cout << "Choose number: ";

  std::string in;
  size_t idx;
  getline(std::cin, in);
  std::stringstream s(in);
  if (const auto is_empty = in == "\n"; !(s >> idx) || idx >= 6 || is_empty) idx = 2;

  auto backend = std::make_shared<autd3::gain::holo::NalgebraBackend>();
  auto amp = 5e3 * autd3::gain::holo::Pascal;
  switch (idx) {
    case 0:
      co_await autd.send_async(m, autd3::gain::holo::SDP(backend)
                                      .add_focus(center + autd3::Vector3(30.0, 0.0, 0.0), amp)
                                      .add_focus(center - autd3::Vector3(30.0, 0.0, 0.0), amp));
      break;
    case 1:
      co_await autd.send_async(m, autd3::gain::holo::GS(backend)
                                      .add_focus(center + autd3::Vector3(30.0, 0.0, 0.0), amp)
                                      .add_focus(center - autd3::Vector3(30.0, 0.0, 0.0), amp));
      break;
    case 2:
      co_await autd.send_async(m, autd3::gain::holo::GSPAT(backend)
                                      .add_focus(center + autd3::Vector3(30.0, 0.0, 0.0), amp)
                                      .add_focus(center - autd3::Vector3(30.0, 0.0, 0.0), amp));
      break;
    case 3:
      co_await autd.send_async(m, autd3::gain::holo::Naive(backend)
                                      .add_focus(center + autd3::Vector3(30.0, 0.0, 0.0), amp)
                                      .add_focus(center - autd3::Vector3(30.0, 0.0, 0.0), amp));
      break;
    case 4:
      co_await autd.send_async(m, autd3::gain::holo::LM(backend)
                                      .add_focus(center + autd3::Vector3(30.0, 0.0, 0.0), amp)
                                      .add_focus(center - autd3::Vector3(30.0, 0.0, 0.0), amp));
      break;
    case 5:
      co_await autd.send_async(m, autd3::gain::holo::Greedy()
                                      .add_focus(center + autd3::Vector3(30.0, 0.0, 0.0), amp)
                                      .add_focus(center - autd3::Vector3(30.0, 0.0, 0.0), amp));
      break;
    default:
      break;
  }
}
