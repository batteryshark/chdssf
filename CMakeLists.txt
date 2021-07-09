cmake_minimum_required(VERSION 3.7)
project(SSFLoader)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)

add_executable(SSFLoader ${SOURCE_FILES})
set_target_properties(SSFLoader PROPERTIES COMPILE_FLAGS "-m32 -w" LINK_FLAGS "-m32 -w ")