project(Main)

# Set external library
set(BUILD_THIRDPART_LIB
 sndfile
)

# Add source files to executable target
add_executable(${PROJECT_NAME} main.cpp)

# Link the static library from subproject1 using its alias sub::lib1
# Link the header only library from subproject2 using its alias sub::lib2
# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    Custom1::Compressor
    Custom1::Reverb
    Custom1::SineWaveGenerator
    Custom1::StereoSurround

    Freeverb::allpass
    Freeverb::comb
    Freeverb::Freeverb
    Freeverb::revmodel

    ${BUILD_THIRDPART_LIB}
)

