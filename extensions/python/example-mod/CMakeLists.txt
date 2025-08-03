cmake_minimum_required(VERSION 3.15...3.26)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C)

set(C_MODULE_NAME _example_mod)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/generated/*.c")

find_package(
  Python
  COMPONENTS Interpreter Development.Module
  REQUIRED)

python_add_library(
  ${C_MODULE_NAME}
  MODULE

  ${SOURCES}

  WITH_SOABI
)

target_include_directories(${C_MODULE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/generated")

message(STATUS "CMake ${CMAKE_VERSION}")
message(STATUS "Python ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")

install(TARGETS ${C_MODULE_NAME} DESTINATION ${SKBUILD_PROJECT_NAME})
