#include "autd3/link/simulator.hpp"

#include "autd3.hpp"
#include "runner.hpp"
#include "util.hpp"
using namespace autd3::driver::geometry;

// Run AUTD Simulator before running this example

int main_() {
  auto autd = autd3::ControllerBuilder()/*
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .add_device(autd3::AUTD3(autd3::Vector3(autd3::AUTD3::DEVICE_WIDTH, 0.0, 0.0)))*/
                  .add_device(autd3::AUTD3(autd3::Vector3(autd3::AUTD3::DEVICE_WIDTH, 0.0, 0.0)).with_rotation(autd3::EulerAngles::from_zyz(0.0*rad, -autd3::pi / 4.0*rad, 0.0*rad)))
                  .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
                  .add_device(autd3::AUTD3(autd3::Vector3(-autd3::AUTD3::DEVICE_WIDTH, 0.0, 0.0)))
                  .add_device(autd3::AUTD3(autd3::Vector3(-2* autd3::AUTD3::DEVICE_WIDTH+10, 0.0, autd3::AUTD3::DEVICE_WIDTH / sqrt(2))).with_rotation(autd3::EulerAngles::from_zyz(0.0*rad, autd3::pi / 4.0*rad, 0.0*rad)))
                  .add_device(autd3::AUTD3(autd3::Vector3(-2* autd3::AUTD3::DEVICE_WIDTH+10, autd3::AUTD3::DEVICE_HEIGHT, autd3::AUTD3::DEVICE_WIDTH / sqrt(2))).with_rotation(autd3::EulerAngles::from_zyz(0.0*rad, autd3::pi / 4.0*rad, 0.0*rad)))
                  .add_device(autd3::AUTD3(autd3::Vector3(-autd3::AUTD3::DEVICE_WIDTH, autd3::AUTD3::DEVICE_HEIGHT, 0.0)))
                  .add_device(autd3::AUTD3(autd3::Vector3(0.0, autd3::AUTD3::DEVICE_HEIGHT, 0.0)))
                  .add_device(autd3::AUTD3(autd3::Vector3(autd3::AUTD3::DEVICE_WIDTH, autd3::AUTD3::DEVICE_HEIGHT, 0.0)).with_rotation(autd3::EulerAngles::from_zyz(0.0*rad, -autd3::pi / 4.0*rad, 0.0*rad)))
                  .open(autd3::link::Simulator::builder(8080).with_timeout(std::chrono::milliseconds(200)));

  // auto autd = autd3::ControllerBuilder()
  //                 .add_device(autd3::AUTD3(autd3::Vector3::Zero()))
  //                 .add_device(autd3::AUTD3(autd3::Vector3(autd3::AUTD3::DEVICE_WIDTH, 0.0, 0.0)))
  //                 .open(autd3::link::Simulator::builder(8080).with_timeout(std::chrono::milliseconds(200)));

  auto res = run(autd);
  res;
}

int main() try { return main_(); } catch (std::exception& e) {
  print_err(e);
  return -1;
}
