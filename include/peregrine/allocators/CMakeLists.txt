set(PEREGRINE_ALLOCATOR_TARGET "PeregrineAllocators-${PEREGRINE_INTERNAL_TARGET_SUFFIX}")

add_library(
        ${PEREGRINE_ALLOCATOR_TARGET} INTERFACE
        Arena.h
        Arena.tpp
        Stack.h
        Stack.tpp
        Policy.h
        TempArena.h
        TempArena.tpp
        Pool.h
        Pool.tpp
        TLSF.h
        TLSF.tpp
)
target_compile_features(${PEREGRINE_ALLOCATOR_TARGET} INTERFACE cxx_std_20)
set_target_properties(${PEREGRINE_ALLOCATOR_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PEREGRINE_ALLOCATOR_TARGET} INTERFACE ${PEREGRINE_LIB_ROOT_DIR})

target_link_libraries(${PEREGRINE_ALLOCATOR_TARGET} INTERFACE
        ${PEREGRINE_MEM_TARGET}
        ${PEREGRINE_TELEMETRY_TARGET}
        ${PEREGRINE_UTILS_TARGET}
)
