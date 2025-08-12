# Project definitions
cmake_minimum_required(VERSION 3.15...3.27)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C)

set(C_MODULE_NAME _example_mod)
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/generated")

find_package(
  Python
  COMPONENTS Interpreter Development.Module
  REQUIRED
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message(STATUS "=== Build Configuration ===")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Python version: ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")
message(STATUS "C module name: ${C_MODULE_NAME}")
message(STATUS "Generated sources directory: ${GENERATED_DIR}")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "===========================")

# ------------------------------------------------------------------------------
# Compiler settings from pyproject.toml
# ------------------------------------------------------------------------------
function(set_from_env_or_default VAR DEFAULT)
  if(DEFINED ENV{${VAR}})
    set(${VAR} $ENV{${VAR}} PARENT_SCOPE)
  elseif(DEFINED ${VAR})
    set(${VAR} ${${VAR}} PARENT_SCOPE)
  else()
    set(${VAR} ${DEFAULT} PARENT_SCOPE)
  endif()
endfunction()

function(set_codec_flag FLAG_NAME ENABLE_STR DISABLE_STR)
  if(DEFINED ENV{A1C_DISABLE_${FLAG_NAME}} OR DEFINED A1C_DISABLE_${FLAG_NAME})
    set(A1C_${FLAG_NAME} "${DISABLE_STR}" PARENT_SCOPE)
  else()
    set(A1C_${FLAG_NAME} "${ENABLE_STR}" PARENT_SCOPE)
  endif()
endfunction()

# Paths
set_from_env_or_default(A1C_PATH "asn1c")
set_from_env_or_default(A1C_SKELETONS_PATH "/usr/local/share/asn1c/")

# Codec generation flags
set_codec_flag(BER "-gen-BER" "-no-gen-BER")
set_codec_flag(OER "-gen-OER" "-no-gen-OER")
set_codec_flag(UPER "-gen-UPER" "-no-gen-UPER")
set_codec_flag(APER "-gen-APER" "-no-gen-APER")
set_codec_flag(XER "-gen-XER" "-no-gen-XER")
set_codec_flag(JER "-gen-JER" "-no-gen-JER")
set_codec_flag(PRINT "-gen-print" "-no-gen-print")

# ASN.1 source files
function(file_exists FILE_VAR)
  if(EXISTS "${FILE_VAR}")
    set(EXISTS_RESULT TRUE PARENT_SCOPE)
  else()
    set(EXISTS_RESULT FALSE PARENT_SCOPE)
  endif()
endfunction()

file(GLOB LOCAL_ASN_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.asn")

if(DEFINED ENV{A1C_ASN_FILES})
  string(REPLACE ";" " " ASN_FILES_FROM_ENV "$ENV{A1C_ASN_FILES}")
  set(A1C_SOURCE_FILES ${ASN_FILES_FROM_ENV})
elseif(DEFINED A1C_ASN_FILES)
  set(A1C_SOURCE_FILES ${A1C_ASN_FILES})
elseif(LOCAL_ASN_FILES)
  message(STATUS "Found ASN.1 source files in current source dir:")
  foreach(f ${LOCAL_ASN_FILES})
    message(STATUS "  - ${f}")
  endforeach()
  set(A1C_SOURCE_FILES "")
  foreach(f ${LOCAL_ASN_FILES})
    list(APPEND A1C_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
  endforeach()
else()
  set(A1C_SOURCE_FILES "")
endif()

if(NOT A1C_SOURCE_FILES)
  message(FATAL_ERROR "No ASN.1 source files found. Please add *.asn files to the source directory or set A1C_ASN_FILES variable.")
else()
  message(STATUS "Using ASN.1 source files from A1C_ASN_FILES:")
  foreach(f ${A1C_SOURCE_FILES})
    message(STATUS "  - ${f}")
  endforeach()
endif()

# ------------------------------------------------------------------------------
# Step 1: Generate C sources and Python stubs using asn1c
# ------------------------------------------------------------------------------
add_custom_command(
  OUTPUT "${GENERATED_DIR}/.stamp"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${GENERATED_DIR}"
  COMMAND ${A1C_PATH} -M "${C_MODULE_NAME}" -D "${GENERATED_DIR}" -S "${A1C_SKELETONS_PATH}" -no-gen-example -gen-python -fcompound-names -no-gen-autotools ${A1C_BER} ${A1C_OER} ${A1C_UPER} ${A1C_PER} ${A1C_XER} ${A1C_JER} ${A1C_PRINT} ${A1C_SOURCE_FILES}
  COMMAND ${CMAKE_COMMAND} -E rename "${GENERATED_DIR}/py_module.pyi" "${GENERATED_DIR}/${C_MODULE_NAME}.pyi"
  COMMAND ${CMAKE_COMMAND} -E touch "${GENERATED_DIR}/.stamp"
  DEPENDS "${A1C_PATH}"
  COMMENT "Generating C sources in ${GENERATED_DIR}"
  VERBATIM
)

add_custom_target(generate_sources ALL
  DEPENDS "${GENERATED_DIR}/.stamp"
)

# ------------------------------------------------------------------------------
# Step 2: Compile the library
# ------------------------------------------------------------------------------
file(GLOB GENERATED_SOURCES "${GENERATED_DIR}/*.c")
message(STATUS "Configured to build with sources from: ${GENERATED_DIR}")

python_add_library(
  ${C_MODULE_NAME}
  MODULE
  ${GENERATED_SOURCES}
  WITH_SOABI
)

add_dependencies(${C_MODULE_NAME} generate_sources)
target_include_directories(${C_MODULE_NAME} PRIVATE "${GENERATED_DIR}")

# ------------------------------------------------------------------------------
# Step 3: Install the compiled module and generated stub
# ------------------------------------------------------------------------------
install(
  TARGETS ${C_MODULE_NAME}
  DESTINATION ${SKBUILD_PROJECT_NAME}
)

# ONLY TOP-LEVEL extension modules are supported
install(
  FILES "${GENERATED_DIR}/${C_MODULE_NAME}.pyi"
  DESTINATION ${SKBUILD_PROJECT_NAME}
)
