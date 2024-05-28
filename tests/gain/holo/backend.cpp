#include <gtest/gtest.h>

#include <autd3/gain/holo/backend_nalgebra.hpp>

TEST(Gain_Holo, BackendNewDelete) {
  auto* backend = new autd3::gain::holo::NalgebraBackend;
  delete backend;
}