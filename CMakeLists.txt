cmake_minimum_required(VERSION 3.10)
project(diabas)
set(CMAKE_CXX_STANDARD 17)

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
  thirdparty/cppfs/source/cppfs/include
  lib/chakracore/include
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
  source/audio/audio_manager.cpp
  source/audio/audio_manager.hpp
  source/app/app.cpp
  source/core/hash.cpp
  source/core/hash.hpp
  source/app/graphics.cpp
  source/game/game.cpp
  source/game/world.cpp
  source/game/world.hpp
  source/graphics/renderer.cpp
  source/graphics/sprite.cpp
  source/graphics/sprite_batch.cpp
  source/script/env.cpp
  source/script/util.cpp
  source/script/script.cpp
  source/script/expose/expose_base.cpp
  source/script/expose/expose_graphics.cpp
  source/script/expose/expose_mod.cpp
  source/mods/loader.cpp
  source/mods/loader.hpp
  source/mods/mod.cpp
  source/mods/mod.hpp
  source/mods/result.cpp
  source/mods/result.hpp
  source/network/network.cpp
  source/network/network.hpp
  source/network/server.cpp
  source/network/server.hpp
  source/network/client.cpp
  source/network/client.hpp
  source/network/packet.cpp
  source/network/packet.hpp
  source/network/packet_handler.cpp
  source/network/packet_handler.hpp
  source/network/packet_header.hpp
  source/network/common.cpp
  source/network/common.hpp
  source/network/side.cpp
  source/network/side.hpp
  source/server/input.cpp
  source/server/input.hpp
  )

set(TEST_SOURCE
  tests/main.test.hpp
  tests/toml.test.cpp
  tests/mods.test.cpp
  tests/packet.test.cpp
  tests/packet_handler.test.cpp
  )

# ChakraCore
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  link_directories(${CMAKE_SOURCE_DIR}/lib/chakracore/lib/debug)
else ()
  link_directories(${CMAKE_SOURCE_DIR}/lib/chakracore/lib/release)
endif ()

# TOML
set(CPPTOML_BUILD_EXAMLES OFF)
include_directories(SYSTEM thirdparty/cpptoml/include)
add_subdirectory(thirdparty/cpptoml)

# alflibcpp
set(ALFLIBCPP_BUILD_TESTS OFF)
include_directories(SYSTEM thirdparty/alflibcpp/include)
add_subdirectory(thirdparty/alflibcpp)

# dlog
add_subdirectory(thirdparty/dlog)

# dutil
add_subdirectory(thirdparty/dutil)

# miniaudio
include_directories(SYSTEM thirdparty/miniaudio)

# Steam Sockets
if (WIN32)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_SOURCE_DIR}/lib/gamenetworkingsockets/lib/debug)
  elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    link_directories(${CMAKE_SOURCE_DIR}/lib/gamenetworkingsockets/lib/relwithdebinfo)
  else ()
    link_directories(${CMAKE_SOURCE_DIR}/lib/gamenetworkingsockets/lib/release)
  endif ()
  include_directories(SYSTEM lib/gamenetworkingsockets/include)
else ()
  include_directories(SYSTEM thirdparty/gamenetworkingsockets/include)
  add_subdirectory(thirdparty/gamenetworkingsockets)
endif ()

# GLM
include_directories(SYSTEM thirdparty/glm)
add_subdirectory(thirdparty/glm)

# GLFW
include_directories(SYSTEM thirdparty/glfw/include)
add_subdirectory(thirdparty/glfw)

# Doctest
include_directories(SYSTEM thirdparty/doctest)
add_subdirectory(thirdparty/doctest)

# Thidparty
include_directories(SYSTEM thirdparty)
aux_source_directory(thirdparty/glad COMMON_SOURCE)

# binary output folder
set(out_dir ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)

# Our executables / builds
add_executable(${PROJECT_NAME} source/main.cpp ${COMMON_SOURCE})
add_executable(server source/main.cpp ${COMMON_SOURCE})
add_executable(test tests/main.test.cpp ${COMMON_SOURCE} ${TEST_SOURCE})

# Compile definitions?
if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
  add_compile_definitions(${PROJECT_NAME} DIB_DEBUG)
endif ()
add_compile_definitions(${PROJECT_NAME} DLOG_TIMESTAMP DLOG_DATESTAMP)
#add_compile_definitions(${PROJECT_NAME} DLOG_LOG_TO_FILE)
if (WIN32)
  add_compile_definitions(${PROJECT_NAME} DIB_TARGET_WINDOWS)
else ()
  add_compile_definitions(${PROJECT_NAME} DIB_TARGET_LINUX)
endif ()

target_compile_definitions(server PRIVATE DIB_IS_SERVER)

# Link libraries
if (WIN32)
  set(DIB_LIBS ws2_32 cpptoml dlog dutil glfw glm gamenetworkingsockets alflibcpp ChakraCore)
else ()
  set(DIB_LIBS pthread cpptoml dlog dutil glfw glm GameNetworkingSockets alflibcpp ChakraCore dl m)
endif ()

# TODO(Filip Björklund): Remove GLFW as a dependency on the server
target_link_libraries(${PROJECT_NAME} ${DIB_LIBS})
target_link_libraries(server ${DIB_LIBS})
target_link_libraries(test doctest ${DIB_LIBS})
