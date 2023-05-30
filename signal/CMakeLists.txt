cmake_minimum_required(VERSION 3.0.0)
project(SIGNAL_PRO1 VERSION 0.1.0)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

set(SRC_LIST main.cpp reverb.cpp stereo_surround.cpp generateSineWave.cpp)

# Add source files to executable target
add_executable(SIGNAL_PRO1 ${SRC_LIST})

# Add include directories for target
target_include_directories(SIGNAL_PRO1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find and link against external library
#find_package(Boost REQUIRED COMPONENTS system)
#target_link_libraries(SIGNAL_PRO1 PRIVATE Boost::system)
find_library(SNDFILE_LIBRARIES NAMES sndfile)
target_link_libraries(SIGNAL_PRO1 PRIVATE ${SNDFILE_LIBRARIES})

# Set build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Install executable to bin directory
install(TARGETS SIGNAL_PRO1 DESTINATION bin)

# Install header files to include directory
install(DIRECTORY include/ DESTINATION include)

# Enable testing
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

#SET_TARGET_PROPERTIES(dllname PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

# Add unit test executable target
#add_executable(MyTest test.cpp)
#target_link_libraries(MyTest PRIVATE MyLibrary)

# Add test case
#add_test(NAME MyTest COMMAND MyTest)