cmake_minimum_required(VERSION 3.10)
project(diabas)
set(CMAKE_CXX_STANDARD 17)


# Where are our headers? Here!
include_directories(
  source/
  thirdparty/
)


# Set extra compile flags here
if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()


# Add our .cpp files, will automatically re-run cmake when new ones are added
set(COMMON_SOURCE
  source/main.cpp
  )


# Thidparty
#aux_source_directory(thirdparty/chif_net/chif_net COMMON_SOURCE)


# Our executables / builds
add_executable(${PROJECT_NAME} diabas ${COMMON_SOURCE})


# Compile definitions?
#add_compile_definitions(coustom_header_data DLOG_MT DLOG_TIMESTAMP)


# Link libraries
if (WIN32)
  target_link_libraries(${PROJECT_NAME} ws2_32)
else()
  target_link_libraries(${PROJECT_NAME} pthread)
endif()
