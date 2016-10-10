project(bloom_search)
cmake_minimum_required(VERSION 2.8)
enable_testing()
include(Compiler/Clang-C)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(OpenSSL REQUIRED)
find_package(Ruby 1.8)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang)

set(CMAKE_C_FLAGS                "-std=gnu99 -march=native -Qunused-arguments -Werror")
set(CMAKE_C_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O4 -DNDEBUG -g")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()


add_library(bloomfilter STATIC
  lib/bloomfilter/bloomfilter.h
  lib/bloomfilter/bloomfilter.c
)

add_library(tree STATIC
  lib/tree/node.h
  lib/tree/node_list.h
  lib/tree/tree.h
  lib/tree/util.h
  lib/tree/positions_vector.h
  lib/tree/matches.h
  lib/tree/node.c  
  lib/tree/query_node.c
  lib/tree/node_list.c
  lib/tree/tree.c
  lib/tree/util.c
  lib/tree/positions_vector.c
  lib/tree/matches.c
)
target_link_libraries(tree
  ${OPENSSL_LIBRARIES}
  bloomfilter
)

add_executable(bloom_search
  bloom_search/main.c
)
target_link_libraries(bloom_search
  bloomfilter
  tree
  pthread
)

add_executable(test_driver
  test/test_driver
  test/test_bloomfilter
  test/test_tree
)
target_link_libraries(test_driver
  bloomfilter
  tree
)

add_library(bfruby SHARED
  binding/bfruby
)
target_link_libraries(bfruby
  ${RUBY_LIBRARY}
  bloomfilter
  tree
)
set_target_properties(bfruby PROPERTIES
  PREFIX ""
  SUFFIX ".bundle"
  INCLUDE_DIRECTORIES "${RUBY_INCLUDE_DIRS}"
)

add_test(bloomfilter ${CMAKE_CURRENT_BINARY_DIR}/test_driver bloomfilter)
add_test(tree        ${CMAKE_CURRENT_BINARY_DIR}/test_driver tree)
