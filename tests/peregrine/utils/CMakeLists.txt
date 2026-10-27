add_library(
        UtilsTests OBJECT
        BitTests.cpp
        ConstantTests.cpp
        HelpersTest.cpp
        MultithreadingTests.cpp
)

target_compile_features(UtilsTests PUBLIC cxx_std_20)

target_include_directories(
        UtilsTests PRIVATE
        ${PEREGRINE_TEST_ROOT_DIR}/include
)


target_link_libraries(
        UtilsTests PRIVATE
        PeregrineTestSetup
)