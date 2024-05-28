#include <gtest/gtest.h>

#include <autd3/driver/common/sampling_config.hpp>
#include <chrono>
#include <numbers>

static constexpr uint32_t SAMPLING_FREQ_DIV_MIN = 512;
static constexpr uint32_t SAMPLING_FREQ_DIV_MAX = 0xFFFFFFFF;
static constexpr double FREQ_MIN = static_cast<double>(autd3::native_methods::FPGA_CLK_FREQ) / SAMPLING_FREQ_DIV_MAX;
static constexpr double FREQ_MAX = static_cast<double>(autd3::native_methods::FPGA_CLK_FREQ) / SAMPLING_FREQ_DIV_MIN;
static constexpr uint64_t PERIOD_MIN = static_cast<uint64_t>(1000000000.0 / autd3::native_methods::FPGA_CLK_FREQ * SAMPLING_FREQ_DIV_MIN);
static constexpr uint64_t PERIOD_MAX = 209715199999;

TEST(DriverCommon, SamplingConfigWithFreqDiv) {
  const auto config = autd3::driver::SamplingConfiguration::from_frequency_division(512);
  ASSERT_EQ(config.frequency_division(), 512);
  ASSERT_EQ(config.frequency(), 40e3);
  ASSERT_EQ(config.period(), std::chrono::microseconds(25));

  ASSERT_THROW((void)autd3::driver::SamplingConfiguration::from_frequency_division(SAMPLING_FREQ_DIV_MIN - 1), autd3::AUTDException);
}

TEST(DriverCommon, SamplingConfigWithFreq) {
  const auto config = autd3::driver::SamplingConfiguration::from_frequency(40e3);
  ASSERT_EQ(config.frequency_division(), 512);
  ASSERT_EQ(config.frequency(), 40e3);
  ASSERT_EQ(config.period(), std::chrono::microseconds(25));

  ASSERT_THROW((void)autd3::driver::SamplingConfiguration::from_frequency(FREQ_MIN - 0.1), autd3::AUTDException);
  ASSERT_THROW((void)autd3::driver::SamplingConfiguration::from_frequency(FREQ_MAX + 0.1), autd3::AUTDException);
}

TEST(DriverCommon, SamplingConfigWithPeriod) {
  const auto config = autd3::driver::SamplingConfiguration::from_period(std::chrono::microseconds(25));
  ASSERT_EQ(config.frequency_division(), 512);
  ASSERT_EQ(config.frequency(), 40e3);
  ASSERT_EQ(config.period(), std::chrono::microseconds(25));

  ASSERT_THROW((void)autd3::driver::SamplingConfiguration::from_period(std::chrono::nanoseconds(PERIOD_MIN - 1)), autd3::AUTDException);
  ASSERT_THROW((void)autd3::driver::SamplingConfiguration::from_period(std::chrono::nanoseconds(PERIOD_MAX + 1)), autd3::AUTDException);
}
