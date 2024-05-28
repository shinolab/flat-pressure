#pragma once

#include <algorithm>
#include <iterator>
#include <thread>
#include <vector>

#include "autd3.hpp"

template <typename L>
inline void flag_test(autd3::Controller<L>& autd) {
  std::cout << "press any key to run fan..." << std::endl;
  std::cin.ignore();

  autd.send(autd3::ConfigureForceFan([](const auto&) { return true; }),
                           autd3::ConfigureReadsFPGAState([](const auto&) { return true; }));

  bool fin = false;
  std::cout << "press any key stop checking FPGA status..." << std::endl;
  auto fin_signal = std::thread([&] {
    std::cin.ignore();
    fin = true;
  });

  const std::vector prompts = {'-', '/', '|', '\\'};
  size_t prompts_idx = 0;
  while (!fin) {
    const auto states = autd.fpga_info();
    std::cout << prompts[prompts_idx++ / 1000 % prompts.size()] << " FPGA Status...\n";
    std::ranges::for_each(std::ranges::views::iota(states.size()), [&states](const size_t i) {
      std::cout << "[" << i << "]: " << (states[i].has_value() ? std::to_string(states[i].value().is_thermal_assert()) : "-") << std::endl;
    });
    std::cout << "\033[" << states.size() + 1 << "A";
  }

  if (fin_signal.joinable()) fin_signal.join();

  autd.send(autd3::ConfigureForceFan([](const auto&) { return false; }),
                           autd3::ConfigureReadsFPGAState([](const auto&) { return false; }));
}
