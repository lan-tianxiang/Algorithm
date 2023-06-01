cmake_minimum_required(VERSION 3.0.0)
project(SIGNAL_PRO1 VERSION 0.1.0)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Set build directory
set(BUILD_DIR Reverb StereoSurround SineWaveGenerator Compressor)

# Set external library
set(BUILD_THIRDPART_LIB sndfile)

# Set internal library
set(BUILD_INTERNAL_LIB Reverb StereoSurround SineWaveGenerator Compressor)

foreach(dir ${BUILD_DIR})
  # Add subdirectories
  add_subdirectory(${dir})
  # Add include directories
  include_directories(${dir})
endforeach()

# Set build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Add source files to executable target
add_executable(SIGNAL_PRO1 main.cpp)

# Find and link against external library
find_library(THIRDPART_LIBRARIES NAMES ${BUILD_THIRDPART_LIB} PATHS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(SIGNAL_PRO1 PRIVATE ${THIRDPART_LIBRARIES})
target_link_libraries(SIGNAL_PRO1 PRIVATE ${BUILD_INTERNAL_LIB})

# Enable testing
enable_testing()

MESSAGE(STATUS "THIS IS BINARY DIR " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "THIS IS SOURCE DIR " ${PROJECT_SOURCE_DIR})

# Add unit test executable target
#add_executable(MyTest test.cpp)
#target_link_libraries(MyTest PRIVATE MyLibrary)

# Add test case
#add_test(NAME MyTest COMMAND MyTest)