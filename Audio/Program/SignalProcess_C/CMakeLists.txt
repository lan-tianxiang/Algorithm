cmake_minimum_required(VERSION 3.0.0)

project(AUDIO_EFFECT_PROCESSING VERSION 0.1.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Set build directory
set(BUILD_DIR
  Main
  Custom1
  Freeverb
)

foreach(dir ${BUILD_DIR})
  # Add subdirectories
  add_subdirectory(${dir})
  # Add include directories
  include_directories(${dir})
endforeach()
