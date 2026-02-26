add_subdirectory(model)
add_subdirectory(utility)
add_subdirectory(service)
add_subdirectory(algorithm)
add_subdirectory(io)
add_library(logger SHARED
        logger.cpp
)
target_link_libraries(logger log4cxx)
