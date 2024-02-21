#include "autd3.hpp"
#include "autd3/link/soem.hpp"
#include "runner.hpp"
#include "util.hpp"

coro::task<int> main_() {
  auto autd = co_await autd3::ControllerBuilder()
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .open_async(autd3::link::RemoteSOEM::builder("127.0.0.1:8080"));
  auto res = co_await run(autd);
  co_return res;
}

int main() try { return sync_wait(main_()); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
