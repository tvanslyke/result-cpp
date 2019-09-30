cmake_minimum_required(VERSION 3.8)

project(result-cpp VERSION 1.0.0 LANGUAGES CXX)

option(RESULT_ENABLE_TESTS "Enable tests." ON)

add_library(result-cpp INTERFACE)

target_include_directories(result-cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(result-cpp INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include/tim/result/Result.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/tim/result/expected.hpp)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

if(RESULT_ENABLE_TESTS)
  # Make test executable
  set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp
                   #${CMAKE_CURRENT_SOURCE_DIR}/tests/extensions.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/assignment.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/relops.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/emplace.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/issues.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/bases.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/observers.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/swap.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/constructors.cpp)

  add_executable(result-tests ${TEST_SOURCES})

  target_link_libraries(result-tests Catch result-cpp)

  set(CXXSTD 17 CACHE STRING "C++ standard to use, default C++14")
  set_property(TARGET result-tests PROPERTY CXX_STANDARD ${CXXSTD})
  if(MSVC)
    target_compile_options(result-tests PRIVATE /W4 /WX)
  else()
    target_compile_options(result-tests PRIVATE -Wall -Wextra -pedantic)
  endif()
endif()
