cmake_minimum_required(VERSION 2.8)

project(kylintest)


if( WIN32 )
    include(C:/tools/cmake_install_libs/serial/share/serial/cmake/serialConfig.cmake)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    find_package(serial REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread")
endif()

include_directories(${serial_INCLUDE_DIRS})


include_directories(asp/inc) 

FILE(GLOB sourcefiles "asp/src/*.c"  "*.cpp" "*.c")
add_executable( kylintest ${sourcefiles} )
target_link_libraries( kylintest serial)
 



