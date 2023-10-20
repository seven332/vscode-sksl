set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_path(SET GOOGLE_TEST_SOURCE_DIR NORMALIZE
    ${CMAKE_CURRENT_SOURCE_DIR}/../externals/googletest
)

add_library(gmock_main STATIC
    ${GOOGLE_TEST_SOURCE_DIR}/googletest/src/gtest-all.cc
    ${GOOGLE_TEST_SOURCE_DIR}/googlemock/src/gmock-all.cc
    ${GOOGLE_TEST_SOURCE_DIR}/googlemock/src/gmock_main.cc
)
target_include_directories(gmock_main PUBLIC
    ${GOOGLE_TEST_SOURCE_DIR}/googletest
    ${GOOGLE_TEST_SOURCE_DIR}/googletest/include
    ${GOOGLE_TEST_SOURCE_DIR}/googlemock
    ${GOOGLE_TEST_SOURCE_DIR}/googlemock/include
)
