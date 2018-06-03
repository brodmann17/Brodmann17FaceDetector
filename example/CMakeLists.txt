cmake_minimum_required(VERSION 3.0.0)

project(example)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package( OpenCV REQUIRED )

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message("Compiling for Linux")
message( STATUS "Architecture: ${ARCHITECTURE}" )

file(GLOB SOURCES *.cpp)
# Include header files


include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(../include/)

# set flags
add_definitions("-std=c++11 -O3")

# Create shared library
add_executable(${PROJECT_NAME} ${SOURCES})

# add libraries 

find_library(BRODMANN17_LIB brodmann17 HINTS ../lib/${ARCHITECTURE})
if ( NOT BRODMANN17_LIB )
	message(FATAL_ERROR "Architecture ${ARCHITECTURE} is not supported!")
endif()
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC ${BRODMANN17_LIB})

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC openblas)
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC pthread)
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC ${PROTOBUF_LIBRARY})
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC ${OpenCV_LIBS})
