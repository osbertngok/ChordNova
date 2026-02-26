add_library(service SHARED
        expansion.cpp
        voiceleading.cpp
)
set_property(TARGET service PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_link_libraries(service PRIVATE chord utility)
