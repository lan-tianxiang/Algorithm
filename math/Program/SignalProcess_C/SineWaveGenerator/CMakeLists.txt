add_library(SineWaveGenerator STATIC
        SineWaveGenerator.cpp
        SineWaveGenerator.h
        )
set_target_properties(SineWaveGenerator PROPERTIES LINKER_LANGUAGE CXX)