cmake_minimum_required(VERSION 3.3)

project(x-test-clip VERSION 0.1 LANGUAGES C)

find_package(X11)

add_executable(x-test-clip
  main.c)

target_include_directories(x-test-clip PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(x-test-clip PRIVATE ${X11_LIBRARIES})
