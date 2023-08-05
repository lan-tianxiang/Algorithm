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
