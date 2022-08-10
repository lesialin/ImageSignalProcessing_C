cmake_minimum_required(VERSION 3.10)
project(isp_pipeline)
find_package( OpenCV REQUIRED )
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/prebuilt>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  $<1:${CMAKE_SOURCE_DIR}/prebuilt>)
add_executable(isp_pipeline
src/isp_core.cpp
src/isp_aaf.cpp
src/isp_cfa.cpp
src/isp_gac.cpp
src/isp_blc.cpp
src/isp_dpc.cpp
src/isp_ccm.cpp
src/isp_awb.cpp
src/isp_csc.cpp
main.cpp)
target_link_libraries(isp_pipeline ${OpenCV_LIBS})
