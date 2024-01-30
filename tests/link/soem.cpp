#ifndef WIN32

#include <gtest/gtest.h>

#include <autd3/controller/builder.hpp>
#include <autd3/controller/controller.hpp>
#include <autd3/link/soem.hpp>

TEST(Link, SOEM) {
  auto link = autd3::link::SOEM::builder()
                  .with_ifname("")
                  .with_buf_size(32)
                  .with_send_cycle(2)
                  .with_sync0_cycle(2)
                  .with_err_handler([](const uint16_t slave, const autd3::link::Status status, const std::string& msg) {})
                  .with_timer_strategy(autd3::native_methods::TimerStrategy::Sleep)
                  .with_sync_mode(autd3::native_methods::SyncMode::FreeRun)
                  .with_state_check_interval(std::chrono::milliseconds(100))
                  .with_timeout(std::chrono::milliseconds(200));

#ifdef RUN_LINK_SOEM
  auto autd =
      autd3::controller::ControllerBuilder().add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero())).open_with_async(std::move(link)).get();

  autd.close();
#else
  (void)link;
#endif
}

TEST(Link, RemoteSOEM) {
  auto link = autd3::link::RemoteSOEM::builder("127.0.0.1:8080").with_timeout(std::chrono::milliseconds(200));
#ifdef RUN_LINK_REMOTE_SOEM

  auto autd =
      autd3::controller::ControllerBuilder().add_device(autd3::driver::AUTD3(autd3::driver::Vector3::Zero())).open_with_async(std::move(link)).get();

  autd.close();
#else
  (void)link;
#endif
}
#endif
