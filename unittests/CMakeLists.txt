FIND_PACKAGE(gtest REQUIRED CONFIG)

ADD_EXECUTABLE(octo-wildcardmatching-cpp-tests
    src/wildcard-path-matcher-tests.cpp
    src/test.cpp
)

# Properties
SET_TARGET_PROPERTIES(octo-wildcardmatching-cpp-tests PROPERTIES CXX_STANDARD 17 POSITION_INDEPENDENT_CODE ON)

TARGET_LINK_LIBRARIES(octo-wildcardmatching-cpp-tests
    octo-wildcardmatching-cpp
    gtest::gtest
    pthread
)

INCLUDE(GoogleTest)
GTEST_ADD_TESTS(TARGET octo-wildcardmatching-cpp-tests)
