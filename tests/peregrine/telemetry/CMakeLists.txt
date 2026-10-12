add_library(
        TelemetryTests OBJECT
)

target_sources(
        TelemetryTests PRIVATE
        ArenaTelemetryTests.cpp
        StackTelemetryTests.cpp
        PoolTelemetryTests.cpp
        TLSFTelemetryTests.cpp
)

target_include_directories(
        TelemetryTests PRIVATE
        ${PEREGRINE_TEST_ROOT_DIR}/include
)


target_link_libraries(
        TelemetryTests PRIVATE
        PeregrineTestSetup
)