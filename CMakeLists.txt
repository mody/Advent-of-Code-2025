cmake_minimum_required(VERSION 3.15)
project(AOC2025)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow -std=c++23 -fno-omit-frame-pointer -ggdb3 -O0")
#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow -std=c++23 -fno-omit-frame-pointer -ggdb3 -O0 -fsanitize=address")
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow  -std=c++23 -fno-omit-frame-pointer -ggdb3 -O3")
#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow  -std=c++23 -fno-omit-frame-pointer -ggdb3 -O3 -fsanitize=address")
#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow  -std=c++23 -fno-omit-frame-pointer -ggdb3 -O3 -fsanitize=memory -fsanitize-memory-track-origins")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error -Wno-error=unused-variable")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS regex REQUIRED)
find_package(range-v3 REQUIRED)
#  find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)
find_package(TBB REQUIRED)

#  find_package(PkgConfig)
#  pkg_check_modules(CRYPTOPP libcryptopp)
#  if (NOT CRYPTOPP_FOUND)
#      pkg_check_modules(CRYPTOPP libcrypto++)
#  endif()
#  if (NOT CRYPTOPP_FOUND)
#      message(FATAL_ERROR "Crypto++ library not found")
#  endif()

set (DEFAULT_LIBS fmt)

function(add_day_exe day_name)
  add_executable(${day_name} src/${day_name}.cc)
  target_compile_features(${day_name} PRIVATE cxx_std_23)

  # DEFAULT_LIBS + any extra libs passed to the function
  target_link_libraries(${day_name} PRIVATE ${DEFAULT_LIBS} ${ARGN})
endfunction()

# example with extra dependency
#  add_day_exe(day2 fmt::fmt)          # day2 needs an extra dep

add_day_exe(day1)
add_day_exe(day2)
add_day_exe(day3)
add_day_exe(day4)
add_day_exe(day5)
add_day_exe(day6)
add_day_exe(day7)
add_day_exe(day8)
add_day_exe(day9)
add_day_exe(day10 tbb)

