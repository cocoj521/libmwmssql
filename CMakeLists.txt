cmake_minimum_required(VERSION 2.6)

project(mwmssql C CXX)

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
 -g
 # -DVALGRIND
 -DLINUX 
 -DSQL_WCHART_CONVERT 
 -DLIB_EXPORTS
 -D_X64_
 -DBUILD_LEGACY_64_BIT_MODE
 #-DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 #-Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 #-march=native
 #-MMD
 -std=c++11
 -fPIC
 -rdynamic
 )
if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
if(CMAKE_BUILD_DYNAMIC_LIB)
  list(APPEND CXX_FLAGS "-shared")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#odbc lib
include_directories(/usr/include)

#freetds lib
link_directories(/usr/local/freetds/lib)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/mwmssql)
include_directories(${PROJECT_SOURCE_DIR}/mwmssql/src)
include_directories(${PROJECT_SOURCE_DIR}/mwmssql/include)
link_directories(${PROJECT_SOURCE_DIR}/mwmssql/lib)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(mwmssql/src)
add_subdirectory(mwmssql/examples)
