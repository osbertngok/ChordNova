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

add_executable(chord_nova_rw_gtest model/model.cpp
        model/pitchclass.cpp
        model/config_test.cpp
        utility/combinatorics_test.cpp
        utility/mixedradix_test.cpp
        service/expansion_test.cpp
        service/voiceleading_test.cpp
        algorithm/validation_test.cpp
        algorithm/progression_test.cpp
        algorithm/sorting_test.cpp
        io/noteparser_test.cpp
        io/midi_test.cpp
        io/database_test.cpp
        utility/midi_encoding_test.cpp
        service/analysis_test.cpp
        service/substitution_test.cpp
        main.cpp)
target_link_libraries(chord_nova_rw_gtest gtest gtest_main chord utility service algorithm io log4cxx logger)