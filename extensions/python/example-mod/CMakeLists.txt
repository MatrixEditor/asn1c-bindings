# Project definitions
cmake_minimum_required(VERSION 3.15...3.27)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C)

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m asn1cpython_support --cmake-dir
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE asn1c-bindings_ROOT
  RESULT_VARIABLE RESULT_CODE
)
if (NOT RESULT_CODE EQUAL 0)
  message(FATAL_ERROR "Failed to get asn1c-bindings cmake directory! Make sure asn1cpython-support is installed")
endif()

find_package(asn1c-bindings CONFIG REQUIRED)

asn1c_add_extension(
  NAME _example_mod
  ASN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/example.asn
)

# Example call to install the extension as a submodule within the project. The
# name is always given WITHOUT the project name.
asn1c_add_extension(
  NAME submodule._test
  SUBMODULE
  CUSTOM_OUTPUT
  # This will place ALL generated files in the src/generated_submodule directory
  # even if we don't specify them here
  PY_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/generated_submodule
  ASN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.asn
)