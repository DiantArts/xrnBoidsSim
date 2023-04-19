# =========================================================================
# extern
# =========================================================================

if (CMAKE_CURRENT_LIST_DIR MATCHES "-src/tests/unit$")
    include(${XRN_BUILD_DIR}/_deps/${XRN_BIN_NAME}/conanbuildinfo.cmake)
else ()
    include(${XRN_BUILD_DIR}/conanbuildinfo.cmake)
endif ()
conan_basic_setup(TARGETS)

foreach(library IN LISTS XRN_LIBRARIES_REQUIREMENTS)
    find_package(${library} REQUIRED)
endforeach()


# =========================================================================
# setup
# =========================================================================

# basic tests
set(XRN_BTESTS_BIN ${XRN_BIN_NAME}_tests)

# find_package(Catch2 REQUIRED)
# include(Catch)

include(CTest)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

if (UNIX)
    include(ProcessorCount)
    ProcessorCount(XRN_NPROC)
    if (NOT XRN_NPROC EQUAL 0)
        set(CTEST_BUILD_FLAGS -j${XRN_NPROC})
        set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${XRN_NPROC})
    endif ()
endif (UNIX)

# auto-runners
if (RUN_UNIT_TESTS)
    add_custom_target(tests_run ALL DEPENDS ${XRN_BTESTS_BIN})
    add_custom_command(
        TARGET tests_run
        COMMENT "Run tests..."
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ctest -C $<CONFIGURATION> --output-on-failures
    )
endif ()



# =========================================================================
# Configuration definition
# =========================================================================

# Define binary
add_library(XRN_${XRN_BIN_NAME}_test_configs OBJECT ${XRN_SOURCES} ${XRN_FRAGMENT} ${XRN_VERTEX} ${XRN_HEADERS})

if (ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        target_compile_options(XRN_${XRN_BIN_NAME}_test_configs PUBLIC -O0 -g3 -fprofile-instr-generate -fcoverage-mapping -fPIC --coverage -fno-inline)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(XRN_${XRN_BIN_NAME}_test_configs PUBLIC -O0 -g3 -fprofile-arcs -ftest-coverage -fPIC --coverage -fno-inline -fno-inline-small-functions -fno-default-inline)
    endif ()
    target_link_libraries(XRN_${XRN_BIN_NAME}_test_configs PUBLIC --coverage)
endif ()

target_link_libraries(
    XRN_${XRN_BIN_NAME}_test_configs
    PUBLIC
        ${XRN_BIN_NAME}_project_options
        ${XRN_BIN_NAME}_project_warnings
        ${XRN_BIN_NAME}_project_dependencies
        GTest::gtest_main
        ${XRN_LIBRARIES_DEPENDENCIES}
)

foreach(library IN LISTS XRN_LIBRARIES_HEADERS)
    include_directories(
        ${${library}_INCLUDE_DIRS}
    )
endforeach()



# =========================================================================
# basic tests
# =========================================================================

file(GLOB_RECURSE XRN_TESTS_SRC *.cpp *.cxx)

if (XRN_TESTS_SRC AND NOT CMAKE_CURRENT_LIST_DIR MATCHES "-src/tests/unit$")

    add_executable(${XRN_BTESTS_BIN} ${XRN_TESTS_SRC})
    target_link_libraries(${XRN_BTESTS_BIN}
        PRIVATE
            XRN_${XRN_BIN_NAME}_test_configs
            ${XRN_BIN_NAME}_project_options
            ${XRN_BIN_NAME}_project_warnings
            ${XRN_BIN_NAME}_project_dependencies
            ${XRN_LIBRARIES_DEPENDENCIES}
    )

    # automatically discover tests that are defined in gtest based test files you can modify the tests.
    gtest_discover_tests(${XRN_BTESTS_BIN})

endif ()



# =========================================================================
# functionnal tests
# =========================================================================

# Provide a simple smoke test to make sure that the CLI works and can display a --help message
# add_test(NAME cli.has_help COMMAND intro --help)

# Provide a test to verify that the version being reported from the application
# matches the version given to CMake. This will be important once you package
# your program. Real world shows that this is the kind of simple mistake that is easy
# to make, but also easy to test for.
# add_test(NAME cli.version_matches COMMAND intro --version)
# set_tests_properties(cli.version_matches PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")
