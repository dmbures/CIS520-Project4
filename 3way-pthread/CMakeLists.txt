cmake_minimum_required (VERSION 2.8)
project(hw4)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror -D_XOPEN_SOURCE=500")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror -Wno-sign-compare -D_XOPEN_SOURCE=500")

# build a dynamic library called libblock_store.so
# note that the prefix lib will be automatically added in the filename.
#add_library(pthread ./pthread.c)

# make an executable
add_executable(pthread_test ./pthread.c)
target_compile_definitions(pthread_test PRIVATE)
#target_link_libraries(${PROJECT_NAME}_test gtest pthread pthread)