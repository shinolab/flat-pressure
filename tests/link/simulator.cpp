#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/controller/controller.hpp>
#include <autd3/link/simulator.hpp>

TEST(Link, Simulator) {
  auto link = autd3::link::Simulator::builder(8080).with_server_ip("127.0.0.1").with_timeout(std::chrono::milliseconds(200));

#ifdef RUN_LINK_SIMULATOR
  auto autd = autd3::controller::ControllerBuilder().add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero())).open(std::move(link));

  autd.close();
#else
  (void)link;
#endif
}
