set(PEREGRINE_LIB_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PEREGRINE_INTERNAL_TARGET_SUFFIX _Internal)
set(PEREGRINE_LIB_NAME Peregrine)

add_library(${PEREGRINE_LIB_NAME} INTERFACE peregrine/Peregrine.h)
add_library(${PEREGRINE_LIB_NAME}::${PEREGRINE_LIB_NAME} ALIAS ${PEREGRINE_LIB_NAME})

set_target_properties(${PEREGRINE_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PEREGRINE_LIB_NAME} INTERFACE cxx_std_20)
set_target_properties(${PEREGRINE_LIB_NAME} PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

add_subdirectory(peregrine/utils)
add_subdirectory(peregrine/memory)
add_subdirectory(peregrine/telemetry)
add_subdirectory(peregrine/allocators)

target_include_directories(
        ${PEREGRINE_LIB_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)


target_link_libraries(
        ${PEREGRINE_LIB_NAME} INTERFACE
        ${PROJECT_NAME}-StrictWarnings # Remove to tests only
        ${PEREGRINE_LIB_NAME}::Internal::Allocators
        ${PEREGRINE_LIB_NAME}::Internal::Memory
        ${PEREGRINE_LIB_NAME}::Internal::Telemetry
        ${PEREGRINE_LIB_NAME}::Internal::Utils
)
