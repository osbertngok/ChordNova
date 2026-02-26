add_library(utility SHARED
        combinatorics.cpp
)
set_property(TARGET utility PROPERTY COMPILE_WARNING_AS_ERROR ON)
