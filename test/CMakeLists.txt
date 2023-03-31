include(FetchContent)

##################### GoogleTest ####################
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(cobweb_test unit/timer_queue.cpp)
target_link_libraries(cobweb_test PRIVATE cobweb gtest gmock gtest_main)
add_test(cobweb-test cobweb_test)
