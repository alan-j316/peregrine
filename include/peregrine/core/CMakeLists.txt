set(PEREGRINE_CORE_TARGET "PeregrineCore-${PEREGRINE_INTERNAL_TARGET_SUFFIX}")

add_library(
        ${PEREGRINE_CORE_TARGET} INTERFACE
        Alias.h
        MemoryManager.h
        MemoryManager.tpp
        AllocatorRegistry.h
)
add_library(${PEREGRINE_LIB_NAME}::Internal::Core ALIAS ${PEREGRINE_CORE_TARGET})

target_compile_features(${PEREGRINE_CORE_TARGET} INTERFACE cxx_std_20)
set_target_properties(${PEREGRINE_CORE_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PEREGRINE_CORE_TARGET} INTERFACE ${PEREGRINE_LIB_ROOT_DIR})

target_link_libraries(${PEREGRINE_CORE_TARGET} INTERFACE
        ${PEREGRINE_LIB_NAME}::Internal::Allocators
        ${PEREGRINE_LIB_NAME}::Internal::Memory
        ${PEREGRINE_LIB_NAME}::Internal::Telemetry
        ${PEREGRINE_LIB_NAME}::Internal::Utils
)
