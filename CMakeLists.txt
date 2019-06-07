cmake_minimum_required(VERSION 3.10)
project(diabas)
set(CMAKE_CXX_STANDARD 17)


# #include "..."
include_directories(
  source/
  )


# Set extra compile flags here
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
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
  source/mods/loader.cpp
  source/mods/loader.hpp
  source/mods/mod_info.hpp
  )


# TOML
set(CPPTOML_BUILD_EXAMLES OFF)
include_directories(SYSTEM thirdparty/cpptoml/include)
add_subdirectory(thirdparty/cpptoml)

# dlog
add_subdirectory(thirdparty/dlog)

# dutil
add_subdirectory(thirdparty/dutil)

# GLFW
include_directories(SYSTEM thirdparty/glfw/include)
add_subdirectory(thirdparty/glfw)

# Thidparty
include_directories(SYSTEM thirdparty)
aux_source_directory(thirdparty/alflib COMMON_SOURCE)
aux_source_directory(thirdparty/glad COMMON_SOURCE)

# binary output folder
set(bin_dir ${PROJECT_SOURCE_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_dir})
#set(CMAKE_BINARY_DIR ${bin_dir})
#set(EXECUTABLE_OUTPUT_PATH ${bin_dir})

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
  target_link_libraries(${PROJECT_NAME} ws2_32 cpptoml dlog dutil glfw)
else()
  target_link_libraries(${PROJECT_NAME} pthread cpptoml dlog dutil glfw)
endif()
