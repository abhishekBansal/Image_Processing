cmake_minimum_required(VERSION 2.6)
SET(EXE_NAME ContrastEnhancement)

# Specify these before creating target
link_directories($ENV{GLUT_LIB} 
				 $ENV{OPENCV_LIB}
) 

include_directories($ENV{OPENCV_INCLUDE}
					$ENV{GLUT_INCLUDE} )

# Solution(in VS) Name
project(${EXE_NAME})

add_executable(${EXE_NAME} ContrastEnhancement.cpp linklist.cpp linklist.h)

target_link_libraries(${EXE_NAME} opencv_core231d opencv_highgui231d opencv_imgproc231d opengl32.lib freeglut.lib glu32.lib)