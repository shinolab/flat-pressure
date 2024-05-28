#include "autd3.hpp"
#include "autd3/link/soem.hpp"
#include "runner.hpp"
#include "util.hpp"

int main_() {
  auto autd = autd3::ControllerBuilder()
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .open(autd3::link::RemoteSOEM::builder("127.0.0.1:8080"));
  auto res = run(autd);
  res;
}

int main() try { return main_(); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
