include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.4
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

function(add_test_target TEST_NAME TEST_SOURCE)
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  target_compile_features(${TEST_NAME} PRIVATE cxx_std_23)
  
  if(MSVC)
    target_compile_options(${TEST_NAME} PRIVATE /utf-8 /W4)
  else()
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  
  target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/HelloWorld)
  target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)
  
  include(Catch)
  catch_discover_tests(${TEST_NAME})
endfunction()

add_test_target(test_quick_sort test_quick_sort.cpp)
