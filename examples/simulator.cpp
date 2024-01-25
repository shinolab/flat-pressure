#include "autd3.hpp"
#include "autd3/link/simulator.hpp"
#include "runner.hpp"
#include "util.hpp"

// Run AUTD Simulator before running this example

coro::task<int> main_() {
  auto autd = co_await autd3::ControllerBuilder()
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .add_device(autd3::AUTD3(autd3::Vector3(autd3::AUTD3::DEVICE_WIDTH, 0.0, 0.0)))
                  .open_with_async(autd3::link::Simulator::builder(8080).with_timeout(std::chrono::milliseconds(200)));
  auto res = co_await run(autd);
  co_return res;
}

int main() try { return sync_wait(main_()); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
