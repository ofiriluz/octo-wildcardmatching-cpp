CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(octo-wildcardmatching-cpp)

# Add CMake options to path
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
INCLUDE(ConfigOptions)
INCLUDE(LoadConanBuildInfo)
LOAD_CONAN_BUILD_INFO()

# Library definition
ADD_LIBRARY(octo-wildcardmatching-cpp STATIC
    src/wildcard-path-matcher.cpp
)

# Properties
SET_TARGET_PROPERTIES(octo-wildcardmatching-cpp PROPERTIES CXX_STANDARD 17 POSITION_INDEPENDENT_CODE ON)

TARGET_COMPILE_OPTIONS(octo-wildcardmatching-cpp
    PRIVATE
        $<$<NOT:$<PLATFORM_ID:Windows>>:-Werror=return-type>
        $<$<NOT:$<PLATFORM_ID:Windows>>:-Werror=switch>
)

TARGET_INCLUDE_DIRECTORIES(octo-wildcardmatching-cpp
    PUBLIC
        # Logger includes
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Installation of the logger
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
)

INSTALL(TARGETS octo-wildcardmatching-cpp
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Unittests
IF(NOT DISABLE_TESTS NOT WIN32)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(unittests)
ENDIF()
