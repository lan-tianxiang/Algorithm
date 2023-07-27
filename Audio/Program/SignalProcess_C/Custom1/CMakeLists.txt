project(Custom)

# Set build directory
set(BUILD_DIR
  Compressor
  #FIR
  Reverb
  SineWaveGenerator
  StereoSurround
)

foreach(dir ${BUILD_DIR})
  # Add subdirectories
  add_subdirectory(${dir})
  # Add include directories
  include_directories(${dir})
endforeach()
