# Set the project name
project (Reverb)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/${PROJECT_NAME}.cpp)
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
