set(PEREGRINE_UTILS_TARGET "PeregrineUtils${PEREGRINE_INTERNAL_TARGET_SUFFIX}")

add_library(
        ${PEREGRINE_UTILS_TARGET} INTERFACE
        Bit.h
        Constants.h
        Helpers.h
        Helpers.tpp
        Multithreading.h
        Preprocessors.h
)
add_library(${PEREGRINE_LIB_NAME}::Internal::Utils ALIAS ${PEREGRINE_UTILS_TARGET})

target_compile_features(${PEREGRINE_UTILS_TARGET} INTERFACE cxx_std_20)
set_target_properties(${PEREGRINE_UTILS_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PEREGRINE_UTILS_TARGET} INTERFACE ${PEREGRINE_LIB_ROOT_DIR})
