FetchContent_Declare(
  gtest
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG        "v1.17.0"
)
FetchContent_MakeAvailable(gtest)

include(GoogleTest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB_RECURSE SPEC_SRCS src/**Spec.cpp)

add_executable(unit_tests ${SPEC_SRCS})

target_link_libraries(unit_tests gtest gtest_main arc)


gtest_discover_tests(unit_tests)
