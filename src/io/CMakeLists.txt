add_library(io SHARED
        noteparser.cpp
        database.cpp
        formatter.cpp
        midi.cpp
)
target_link_libraries(io chord algorithm)
