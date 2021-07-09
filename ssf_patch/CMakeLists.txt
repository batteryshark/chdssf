cmake_minimum_required(VERSION 3.7)
project(ssf_patch)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES global.c global.h main.c main.h fs.c fs.h chd_helper.h sptd.c sptd.h)
add_library(ssf_patch SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(ssf_patch PROPERTIES PREFIX "")
set_target_properties(ssf_patch PROPERTIES COMPILE_FLAGS "-m32 -w" LINK_FLAGS "-m32 -w ")