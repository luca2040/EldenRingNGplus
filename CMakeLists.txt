cmake_minimum_required(VERSION 3.9)

project(eldenringNGPlus
  VERSION "1.0.0"
  LANGUAGES C CXX)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

include(FetchContent)

FetchContent_Declare(scanlib
  GIT_REPOSITORY https://github.com/luca2040/scanlib.git
  GIT_TAG 523df76fad17f226d865ae4cfbc0cde24a19da01)

FetchContent_Declare(tinyfiledialogs
  GIT_REPOSITORY https://github.com/hghpublic/vareille-tinyfiledialogs.git
  GIT_TAG 4691611e889921d88d70918a8c59c2c664d7010a)

FetchContent_MakeAvailable(scanlib tinyfiledialogs)

add_library(eldenringNGPlus SHARED
  src/dllmain.cpp)

add_library(tinyfiledialogs STATIC
  ${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c)

target_include_directories(tinyfiledialogs PUBLIC
  ${tinyfiledialogs_SOURCE_DIR}
)

set_target_properties(eldenringNGPlus PROPERTIES
  OUTPUT_NAME "eldenring_ng_plus")

target_link_libraries(eldenringNGPlus
  PRIVATE scanlib tinyfiledialogs)
