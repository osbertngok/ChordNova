cmake_minimum_required(VERSION 3.14)
project(chordnovarw)

# Linux
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13")
        message(WARNING "Current gcc version is ${CMAKE_CXX_COMPILER_VERSION}, using std for log4cxx")
        set(LOG4CXX_FORMAT_NAMESPACE std CACHE STRING "std" FORCE)
    else()
        message(WARNING "Current gcc version is ${CMAKE_CXX_COMPILER_VERSION}, using fmt for log4cxx")
        find_package(fmt 7.1 REQUIRED)
        if (fmt_FOUND)
            message(STATUS "found fmt in ${fmt_INCLUDE_DIR} ${fmt_LIBRARY}")
            include_directories(${fmt_INCLUDE_DIR} ${fmt_LIBRARY})
        else()
            message(FATAL_ERROR "Cannot find fmt")
        endif()
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")

endif()

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

#FetchContent_Declare(
#        apr
#        URL https://github.com/apache/apr/archive/refs/tags/1.7.5.zip
#        DOWNLOAD_EXTRACT_TIMESTAMP false
#)
#FetchContent_MakeAvailable(apr)
#include_directories(${apr_SOURCE_DIR}/src/main/include ${apr_BINARY_DIR}/src/main/include)

FetchContent_Declare(
        log4cxx
        URL https://github.com/apache/logging-log4cxx/archive/refs/tags/rel/v1.1.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP false
)
#FetchContent_Declare(
#        log4cxx
#        GIT_REPOSITORY https://github.com/apache/logging-log4cxx.git
#        GIT_TAG        master  # Or specify a commit/tag
#)
FetchContent_MakeAvailable(log4cxx)
message(STATUS Source Dir: ${log4cxx_SOURCE_DIR})
include_directories(${log4cxx_SOURCE_DIR}/src/main/include ${log4cxx_BINARY_DIR}/src/main/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/main)

add_subdirectory(main)
add_subdirectory(maintest)

add_subdirectory(src)
add_subdirectory(test)

add_custom_target(doc
    COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)