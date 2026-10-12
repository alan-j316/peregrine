set(PEREGRINE_LIB_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PEREGRINE_INTERNAL_TARGET_SUFFIX _Internal)
set(PEREGRINE_LIB_NAME Peregrine)
include(peregrine/Peregrine.cmake)

add_library(${PEREGRINE_LIB_NAME} STATIC ${HeaderFiles})
add_library(${PEREGRINE_LIB_NAME}::${PEREGRINE_LIB_NAME} ALIAS ${PEREGRINE_LIB_NAME})

set_target_properties(${PEREGRINE_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PEREGRINE_LIB_NAME} PUBLIC cxx_std_20)
set_target_properties(${PEREGRINE_LIB_NAME} PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

add_subdirectory(peregrine/utils)
add_subdirectory(peregrine/memory)
add_subdirectory(peregrine/telemetry)
add_subdirectory(peregrine/allocators)

target_include_directories(
        ${PEREGRINE_LIB_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
        ${PEREGRINE_LIB_NAME} PUBLIC
        ${PROJECT_NAME}-StrictWarnings
        ${PEREGRINE_ALLOCATOR_TARGET}
        ${PEREGRINE_MEM_TARGET}
        ${PEREGRINE_TELEMETRY_TARGET}
        ${PEREGRINE_UTILS_TARGET}
)

#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES
#        ${HeaderFiles}
#)
#
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CMake Files" FILES
#        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/allocators/Allocators.cmake
#        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/memory/CMakeLists.txt
#        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/telemetry/Telemetry.cmake
#        ${CMAKE_CURRENT_SOURCE_DIR}/peregrine/utils/Utils.cmake
#)