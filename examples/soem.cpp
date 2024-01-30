#include <iostream>

#include "autd3.hpp"
#include "autd3/link/soem.hpp"
#include "runner.hpp"
#include "util.hpp"

coro::task<int> main_() {
  auto autd = co_await autd3::ControllerBuilder()
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .open_with_async(autd3::link::SOEM::builder().with_err_handler(
                      [](const uint16_t slave, const autd3::link::Status status, const std::string& msg) {
                        switch (status) {
                          case autd3::link::Status::Error:
                            std::cout << "Error [" << slave << "]: " << msg << std::endl;
                            break;
                          case autd3::link::Status::Lost:
                            std::cout << "Lost [" << slave << "]: " << msg << std::endl;
                            // You can also wait for the link to recover, without exiting the process
                            exit(-1);
                          case autd3::link::Status::StateChanged:
                            std::cout << "StateChanged [" << slave << "]: " << msg << std::endl;
                            break;
                        }
                      }));
  auto res = co_await run(autd);
  co_return res;
}

int main() try { return sync_wait(main_()); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
