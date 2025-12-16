cmake_minimum_required(VERSION 3.15)
project(AOC2025)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow -std=c++20 -fno-omit-frame-pointer -ggdb3 -O0")
#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow -std=c++20 -fno-omit-frame-pointer -ggdb3 -O0 -fsanitize=address")
#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow  -std=c++20 -fno-omit-frame-pointer -ggdb3 -O3")
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow  -std=c++20 -fno-omit-frame-pointer -ggdb3 -O3 -fsanitize=address")
#  set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow  -std=c++20 -fno-omit-frame-pointer -ggdb3 -O3 -fsanitize=memory -fsanitize-memory-track-origins")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error -Wno-error=unused-variable")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS regex REQUIRED)
find_package(range-v3 REQUIRED)
#  find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)
#  find_package(TBB REQUIRED)

#  find_package(PkgConfig)
#  pkg_check_modules(CRYPTOPP libcryptopp)
#  if (NOT CRYPTOPP_FOUND)
#      pkg_check_modules(CRYPTOPP libcrypto++)
#  endif()
#  if (NOT CRYPTOPP_FOUND)
#      message(FATAL_ERROR "Crypto++ library not found")
#  endif()

set (DEFAULT_LIBS fmt)

add_executable(day1 src/day1.cc)
target_compile_features(day1 PRIVATE cxx_std_23)
target_link_libraries(day1 PRIVATE ${DEFAULT_LIBS})

add_executable(day2 src/day2.cc)
target_compile_features(day2 PRIVATE cxx_std_23)
target_link_libraries(day2 PRIVATE ${DEFAULT_LIBS})
