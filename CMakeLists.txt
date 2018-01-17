cmake_minimum_required (VERSION 3.2)
project (pgspin)

set (pgspin_VERSION_MAJOR 0)
set (pgspin_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
 
include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include")

set (CMAKE_CXX_STANDARD 11)
set(Boost_USE_MULTITHREAD 	ON)
find_package(Boost 1.53.0 COMPONENTS system thread program_options REQUIRED)

if(Boost_FOUND)

	include_directories(${Boost_INCLUDE_DIRS})
	add_executable(pgspin src/main.C)
	target_link_libraries(pgspin ${Boost_LIBRARIES})

endif()




