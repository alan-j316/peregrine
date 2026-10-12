message(STATUS "[Peregrine]: Unit Tests Enabled(PMM_TESTS).")
add_executable(PeregrineTests)
add_executable(PeregrineDeathTests)

set(PEREGRINE_TEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(PeregrineTestSetup INTERFACE
        include/DoxygenGroups.h
        include/Mocks.h
        include/Types.h
        include/Utils.h
)

target_link_libraries(
        PeregrineTestSetup
        INTERFACE
        Peregrine::Peregrine
        GTest::gtest_main
        ${PROJECT_NAME}-StrictWarnings

)


# Enable death tests only in debug mode
target_compile_definitions(PeregrineTestSetup INTERFACE
        $<$<CONFIG:Debug>:ENABLE_PMM_DEATH_TESTS>
)


# Enables GTest's Friend tests
# Unless the consuming application has GTest imported, DON'T enable this!!
target_compile_definitions(PeregrineTestSetup INTERFACE ENABLE_PMM_TESTS ENABLE_PMM_TELEMETRY)


target_include_directories(
        PeregrineTestSetup INTERFACE
        $<BUILD_INTERFACE:${PEREGRINE_TEST_ROOT_DIR}/include>
)

add_subdirectory(peregrine/allocators)
add_subdirectory(peregrine/telemetry)
add_subdirectory(peregrine/utils)
add_subdirectory(peregrine/memory)


target_link_libraries(
        PeregrineTests PRIVATE
        PeregrineTestSetup

        AllocatorTests
        TelemetryTests
        UtilsTests
        MemoryTests
)


target_link_libraries(
        PeregrineDeathTests PRIVATE
        PeregrineTestSetup

        AllocatorDeathTests
        MemoryDeathTests
)

include(GoogleTest)
gtest_discover_tests(PeregrineTests)
gtest_discover_tests(PeregrineDeathTests)

set_target_properties(AllocatorTests
        TelemetryTests
        UtilsTests
        MemoryTests
        AllocatorDeathTests
        MemoryDeathTests
        PROPERTIES FOLDER "Test Intermediates/"
)