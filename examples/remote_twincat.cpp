#include "autd3.hpp"
#include "autd3/link/twincat.hpp"
#include "runner.hpp"
#include "util.hpp"

coro::task<int> main_() {
  auto autd = co_await autd3::ControllerBuilder()
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .open_async(autd3::link::RemoteTwinCAT::builder("your TwinCATAUTDServer AMS net id (e.g. 172.16.99.2.1.1)"));
  auto res = co_await run(autd);
  co_return res;
}

int main() try { sync_wait(main_()); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
