add_library(algorithm SHARED
        validation.cpp
        sorting.cpp
        progression.cpp
        analysis.cpp
        substitution.cpp
)
target_link_libraries(algorithm chord utility service)
