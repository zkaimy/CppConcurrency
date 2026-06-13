add_executable(01_std_thread "main.cpp")

target_compile_features(01_std_thread PRIVATE cxx_std_23)

if(MSVC)
  target_compile_options(01_std_thread PRIVATE /utf-8 /W4)
else()
  target_compile_options(01_std_thread PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)
target_link_libraries(01_std_thread PRIVATE Threads::Threads spdlog::spdlog)
