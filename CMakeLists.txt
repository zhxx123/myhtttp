# version 
cmake_minimum_required(VERSION 2.6)
# projece name
project(HTTP)

# head file path
INCLUDE_DIRECTORIES(
src
src/compat
src/config
${CMAKE_CURRENT_SOURCE_DIR}
)

#4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
SET(SOURCE_FILES ${DIR_SRCS})
 
set(PROJECT_NAME main)
set(CXX_FLAGS -g -w)
set(CMAKE_CXX_COMPILER "g++")
add_compile_options(-std=c++11)
add_definitions(-DHAVE_CONFIG_H -DHAVE_ENDIAN_H)
message(STATUS ${PROJECT_NAME})
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


link_directories(${PROJECT_SOURCE_DIR}/.libs)
LINK_DIRECTORIES(/usr/local/lib)
LINK_LIBRARIES(univalue)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} pthread ssl crypto boost_system boost_thread event)
