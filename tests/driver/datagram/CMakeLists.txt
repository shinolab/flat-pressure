target_sources(test_autd3 PRIVATE
  clear.cpp
  debug.cpp
  force_fan.cpp
  reads_fpga_state.cpp
  silencer.cpp
  synchronize.cpp
)

add_subdirectory(gain)
add_subdirectory(stm)
add_subdirectory(modulation)