################################################
# Normal Tests
add_library(PMMCoreTests OBJECT)

target_sources(
        PMMCoreTests
        PRIVATE
        AliasTests.cpp
        PoolRegistry.cpp

        memory-manager/InternalMemoryManagerTests.cpp
        memory-manager/ExternalMemoryManagerTests.cpp
)

# Disable padded warning for Mock.h
target_compile_options(PMMCoreTests PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd4324>)

target_include_directories(
        PMMCoreTests PRIVATE
        ${PEREGRINE_TEST_ROOT_DIR}/include
)

target_link_libraries(
        PMMCoreTests PRIVATE
        PeregrineTestSetup
)


################################################
# Death Tests

add_library(PMMCoreDeathTests OBJECT)

target_sources(
        PMMCoreDeathTests
        PRIVATE
        memory-manager/DeathTests.cpp
)

target_include_directories(
        PMMCoreDeathTests PRIVATE
        ${PEREGRINE_TEST_ROOT_DIR}/include
)

target_link_libraries(
        PMMCoreDeathTests PRIVATE
        PeregrineTestSetup
)