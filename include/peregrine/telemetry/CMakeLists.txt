set(PEREGRINE_TELEMETRY_TARGET "PeregrineTelemetry${PEREGRINE_INTERNAL_TARGET_SUFFIX}")

add_library(
        ${PEREGRINE_TELEMETRY_TARGET} INTERFACE
        ArenaTelemetry.h
        ArenaTelemetry.tpp
        StackTelemetry.h
        StackTelemetry.tpp
        PoolTelemetry.h
        PoolTelemetry.tpp
        TLSFTelemetry.h
        TLSFTelemetry.tpp
)
target_compile_features(${PEREGRINE_TELEMETRY_TARGET} INTERFACE cxx_std_20)
set_target_properties(${PEREGRINE_TELEMETRY_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PEREGRINE_TELEMETRY_TARGET} INTERFACE ${PEREGRINE_LIB_ROOT_DIR})
