cmake_minimum_required(VERSION 3.10)
project(diabas)
set(CMAKE_CXX_STANDARD 17)


# Where are our headers? Here!
# #include "..."
include_directories(
  source/
  )

# #include <...>
include_directories(SYSTEM
  thirdparty
  thirdparty/glfw/include
  thirdparty/fmt/include
  thirdparty/dlog/source
  )

# Set extra compile flags here
if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDIB_DEBUG")


# Add our .cpp files, will automatically re-run cmake when new ones are added
set(COMMON_SOURCE
  source/game.cpp
  source/main.cpp
  source/app/app.cpp
  source/app/graphics/graphics.cpp
  source/core/string.cpp
  )

# GLFW
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/dlog)

# Thidparty
aux_source_directory(thirdparty/alflib COMMON_SOURCE)
aux_source_directory(thirdparty/glad COMMON_SOURCE)
aux_source_directory(thirdparty/dutil COMMON_SOURCE)

# Our executables / builds
add_executable(${PROJECT_NAME} ${COMMON_SOURCE})

# Compile definitions?
add_compile_definitions(${PROJECT_NAME} DIB_DEBUG)
add_compile_definitions(${PROJECT_NAME} SDL_MAIN_HANDLED)
add_compile_definitions(${PROJECT_NAME} DLOG_TIMESTAMP DLOG_DATESTAMP)
#add_compile_definitions(${PROJECT_NAME} DLOG_LOG_TO_FILE)
if (WIN32)
  add_compile_definitions(${PROJECT_NAME} DIB_TARGET_WINDOWS)
else()
  add_compile_definitions(${PROJECT_NAME} DIB_TARGET_LINUX)
endif()

# Link libraries
if (WIN32)
  target_link_libraries(${PROJECT_NAME} ws2_32 dlog glfw)
else()
  target_link_libraries(${PROJECT_NAME} pthread dlog glfw)
endif()
