cmake_minimum_required(VERSION 3.8)

project(result-cpp VERSION 1.0.0 LANGUAGES CXX)

option(RESULT_ENABLE_TESTS "Enable tests." ON)

add_library(result-cpp INTERFACE)

set(CXXSTD 17 CACHE STRING "C++ standard to use, default C++14")

enable_testing()

target_include_directories(result-cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(result-cpp INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include/tim/result/Result.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/tim/result/expected.hpp)


if(RESULT_ENABLE_TESTS)

	# Prepare "Catch" library for other executables
	set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

	add_library(Catch INTERFACE)
	target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

	function(AddFailingTest NAME SOURCES)
		add_executable(test_${NAME} ${SOURCE})
		set_property(TARGET test_${NAME} PROPERTY CXX_STANDARD ${CXXSTD})
		target_link_libraries(test_${NAME} Catch result-cpp)
		set_target_properties(test_${NAME} PROPERTIES
			EXCLUDE_FROM_ALL TRUE
			EXCLUDE_FROM_DEFAULT_BUILD TRUE
			LINKER_LANGUAGE CXX)
		add_test(NAME ${NAME}
			COMMAND ${CMAKE_COMMAND} --build . --target test_${NAME}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
		set_tests_properties(${NAME} PROPERTIES WILL_FAIL TRUE)
	endfunction(AddFailingTest)

	# Make test executable
	set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/assignment.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/relops.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/emplace.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/issues.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/bases.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/observers.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/swap.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/status.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/bad_result_access.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/destructor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/result/constructors.cpp)

	AddFailingTest(copy_assign_error_assign_fail ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/copy/copy-assign-error-assign.fail.cpp)
	AddFailingTest(copy_assign_error_ctor_fail   ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/copy/copy-assign-error-ctor.fail.cpp)
	AddFailingTest(copy_assign_value_assign_fail ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/copy/copy-assign-value-assign.fail.cpp)
	AddFailingTest(copy_assign_value_ctor_fail   ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/copy/copy-assign-value-ctor.fail.cpp)
	AddFailingTest(copy_ctor_error_fail          ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/copy/copy-ctor-error.fail.cpp)
	AddFailingTest(copy_ctor_value_fail          ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/copy/copy-ctor-value.fail.cpp)
	AddFailingTest(result_array_error_fail                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/array-error.fail.cpp)
	AddFailingTest(result_array_fail                           ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/array.fail.cpp)
	AddFailingTest(result_error_error_fail                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/error-error.fail.cpp)
	AddFailingTest(result_error_fail                           ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/error.fail.cpp)
	AddFailingTest(result_function_error_fail                  ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/function-error.fail.cpp)
	AddFailingTest(result_function_fail                        ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/function.fail.cpp)
	AddFailingTest(result_in_place_error_t_const_fail          ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_error_t.const.fail.cpp)
	AddFailingTest(result_in_place_error_t_const_volatile_fail ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_error_t.const-volatile.fail.cpp)
	AddFailingTest(result_in_place_error_t_fail                ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_error_t.fail.cpp)
	AddFailingTest(result_in_place_error_t_volatile_fail       ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_error_t.volatile.fail.cpp)
	AddFailingTest(result_in_place_t_const_fail                ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_t.const.fail.cpp)
	AddFailingTest(result_in_place_t_const_volatile_fail       ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_t.const-volatile.fail.cpp)
	AddFailingTest(result_in_place_t_fail                      ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_t.fail.cpp)
	AddFailingTest(result_in_place_t_volatile_fail             ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/in_place_t.volatile.fail.cpp)
	AddFailingTest(result_reference_error_fail                 ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/reference-error.fail.cpp)
	AddFailingTest(result_reference_fail                       ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/reference.fail.cpp)
	AddFailingTest(result_void_error_fail                      ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/void-error.fail.cpp)
	AddFailingTest(result_const_void_error_fail                ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/const-void-error.fail.cpp)
	AddFailingTest(result_volatile_void_error_fail             ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/volatile-void-error.fail.cpp)
	AddFailingTest(result_const_volatile_void_error_fail       ${CMAKE_CURRENT_SOURCE_DIR}/tests/result/fail/types/const-volatile-void-error.fail.cpp)

	add_executable(result-tests ${TEST_SOURCES})

	target_link_libraries(result-tests Catch result-cpp)

	set_property(TARGET result-tests PROPERTY CXX_STANDARD ${CXXSTD})
	if(MSVC)
		target_compile_options(result-tests PRIVATE /W4 /WX)
	else()
		target_compile_options(result-tests PRIVATE -Wall -Wextra -pedantic)
	endif()

	add_test(NAME ResultTests COMMAND ./result-tests)

endif()



