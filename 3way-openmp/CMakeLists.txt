cmake_minimum_required (VERSION 3.9)
project(hw4)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror -D_XOPEN_SOURCE=500")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror -Wno-sign-compare -D_XOPEN_SOURCE=500")

# build a dynamic library called libblock_store.so
# note that the prefix lib will be automatically added in the filename.
find_package(OpenMP REQUIRED)

# make an executable
add_executable(openmp_test ./working_openmp.c)
target_compile_definitions(openmp_test PRIVATE)
target_link_libraries(openmp_test PRIVATE OpenMP::OpenMP_CXX)
