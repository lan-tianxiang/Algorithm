# Set the parent project name
set(PARENT_PROJECT_NAME "${PROJECT_NAME}")

# Set the parent project source dir
cmake_path(SET PARENT_PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")

# Set the current project source dir
cmake_path(SET CURRENT_PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the current project dir name
cmake_path(GET CURRENT_PROJECT_SOURCE_DIR FILENAME CURRENT_PROJECT_DIR_NAME)

# Set the parent project dir name
cmake_path(GET PARENT_PROJECT_SOURCE_DIR FILENAME PARENT_PROJECT_DIR_NAME)

# Set the project name
project (${PARENT_PROJECT_DIR_NAME}_${CURRENT_PROJECT_DIR_NAME})

# Get dir list
file(GLOB srcs_cpp RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

#use this to remove the CMakeLists.txt file from the list
list(REMOVE_ITEM srcs_cpp "CMakeLists.txt")

foreach(dir ${srcs_cpp})
  # output message
  message(STATUS "Found source file: ${dir}")
  # Add subdirectories
  add_subdirectory(${dir})
  # Add include directories
  include_directories(${dir})
endforeach()
