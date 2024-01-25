#include <iostream>

#include "autd3.hpp"
#include "autd3/link/soem.hpp"
#include "runner.hpp"
#include "util.hpp"

void on_err(const char* msg) { std::cerr << "[SOEM Err]" << msg << std::endl; }

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
                  .open_with_async(autd3::link::SOEM::builder().with_on_err(&on_err).with_on_lost(&on_lost));
  auto res = co_await run(autd);
  co_return res;
}

int main() try { return sync_wait(main_()); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
