add_executable(HelloWorld "HelloWorld.cpp" "HelloWorld.h" "sequential_quick_sort.h")

target_compile_features(HelloWorld PRIVATE cxx_std_23)

if(MSVC)
  target_compile_options(HelloWorld PRIVATE /utf-8 /W4)
else()
  target_compile_options(HelloWorld PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(OpenMP REQUIRED)
target_link_libraries(HelloWorld PRIVATE OpenMP::OpenMP_CXX)
