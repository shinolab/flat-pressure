#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/controller/controller.hpp>
#include <autd3/link/twincat.hpp>

TEST(Link, TwinCAT) {
  auto link = autd3::link::TwinCAT::builder().with_timeout(std::chrono::milliseconds(200));

#ifdef RUN_LINK_TWINCAT
  auto autd =
      autd3::controller::ControllerBuilder().add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero())).open_async(std::move(link)).get();

  autd.close();
#else
  (void)link;
#endif
}

TEST(Link, RemoteTwinCAT) {
  auto link = autd3::link::RemoteTwinCAT::builder("xxx.xxx.xxx.xxx.xxx.xxx")
                  .with_server_ip("127.0.0.1")
                  .with_client_ams_net_id("xxx.xxx.xxx.xxx.xxx.xxx")
                  .with_timeout(std::chrono::milliseconds(200));

#ifdef RUN_LINK_REMOTE_TWINCAT
  auto autd =
      autd3::controller::ControllerBuilder().add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero())).open_async(std::move(link)).get();

  autd.close();
#else
  (void)link;
#endif
}
