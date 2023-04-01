
cmake_minimum_required(VERSION 3.25)

project(cobweb
  VERSION 0.1.0
  DESCRIPTION "A header-only constant time timer queue."
)
set(CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE)

add_library(cobweb INTERFACE)
target_include_directories(cobweb INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(cobweb INTERFACE
  cxx_std_20
)

add_executable(app src/app.cpp)
target_link_libraries(app PRIVATE
  cobweb
)

# Public options
option(COBWEB_ENABLE_TESTS "Set ON to build tests" OFF)

if (${COBWEB_ENABLE_TESTS})
  message(STATUS "Tests are included in this build")
  enable_testing()
  add_subdirectory(test)
endif ()
