target_sources(test_autd3 PRIVATE
  controller.cpp
  firmware_info.cpp
)

add_subdirectory(common)
add_subdirectory(datagram)
add_subdirectory(geometry)
