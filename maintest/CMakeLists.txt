include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(chord_nova_gtest legacymodel.cpp
        find_vec.cpp
        main.cpp
        parse_notes.cpp
        get_progression.cpp)
target_link_libraries(chord_nova_gtest gtest gtest_main legacychord log4cxx)
