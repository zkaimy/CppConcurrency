add_executable(02_std_mutex
  "main.cpp"
  "basic_mutex.cpp"
  "recursive_mutex.cpp"
  "timed_mutex.cpp"
  "recursive_timed_mutex.cpp"
  "shared_mutex.cpp"
  "lock_guards.cpp"
  "unique_lock.cpp"
  "lock_algorithms.cpp"
)

target_compile_features(02_std_mutex PRIVATE cxx_std_23)

if(MSVC)
  target_compile_options(02_std_mutex PRIVATE /utf-8 /W4)
else()
  target_compile_options(02_std_mutex PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)
target_link_libraries(02_std_mutex PRIVATE Threads::Threads spdlog::spdlog)
