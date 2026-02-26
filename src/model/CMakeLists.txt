add_library(chord SHARED
        bigramchordstatistics.cpp
        chordstatistics.cpp
        circleoffifths.cpp
        orderedchord.cpp
        pitch.cpp
        pitchclass.cpp
        pitchiterable.cpp
        pitchset.cpp
        ../utility.cpp
)
set_property(TARGET chord PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_link_libraries( chord PRIVATE log4cxx)