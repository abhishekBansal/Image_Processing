cmake_minimum_required(VERSION 2.6)
SET(EXE_NAME HistogramEqualization)

# Specify these before creating target
link_directories($ENV{OPENCV_LIB}) 

include_directories($ENV{OPENCV_INCLUDE})

# Solution(in VS) Name
project(${EXE_NAME})

add_executable(${EXE_NAME} HistogramEqualization.cpp)

target_link_libraries(${EXE_NAME} opencv_core231d opencv_highgui231d opencv_imgproc231d)