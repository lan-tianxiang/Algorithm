add_library(Reverb STATIC
        reverb.cpp
        reverb.h
        )
set_target_properties(Reverb PROPERTIES LINKER_LANGUAGE CXX)