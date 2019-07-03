cmake_minimum_required(VERSION 3.12)
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

## ========================================================================== ##
## Source Files
## ========================================================================== ##

# Add our .cpp files, will automatically re-run cmake when new ones are added
set(COMMON_SOURCE
  source/audio/audio_manager.cpp
  source/audio/audio_manager.hpp
  source/core/hash.cpp
  source/core/hash.hpp
  source/core/memory.cpp
  source/core/value_store.cpp
  source/core/value_store.hpp
  source/game/chat/chat.cpp
  source/game/chat/chat.hpp
  source/game/chat/chat_message.cpp
  source/game/chat/chat_message.hpp
  source/game/ecs/entity_manager.cpp
  source/game/ecs/entity_manager.hpp
  source/game/ecs/components/player_data_component.hpp
  source/game/ecs/systems/player_system.cpp
  source/game/ecs/systems/player_system.hpp
  source/game/ecs/systems/generic_system.hpp
  source/game/ecs/systems/npc_system.cpp
  source/game/ecs/systems/npc_system.hpp
  source/game/resource.cpp
  source/game/resource.hpp
  source/game/terrain.cpp
  source/game/terrain.hpp
  source/game/world.cpp
  source/game/world.hpp
  source/game/item/item.cpp
  source/game/item/item_registry.cpp
  source/game/item/item_stack.cpp
  source/game/tile/tile.cpp
  source/game/tile/tile_entity.cpp
  source/game/tile/tile_registry.cpp
  source/game/mods/script/expose_item.cpp
  source/game/mods/script/expose_mod.cpp
  source/game/mods/script/expose_tile.cpp
  source/game/mods/script/expose_world.cpp
  source/game/mods/script/mod_script.cpp
  source/game/mods/loader.cpp
  source/game/mods/loader.hpp
  source/game/mods/mod.cpp
  source/game/mods/mod.hpp
  source/game/mods/result.cpp
  source/game/mods/result.hpp
  source/script/env.cpp
  source/script/util.cpp
  source/script/expose/expose_base.cpp
  source/script/expose/expose_graphics.cpp
  source/script/expose/expose_network.cpp
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
  source/network/connection_id.hpp
  source/network/connection_id.cpp
  )

## -------------------------------------------------------------------------- ##

set(TEST_SOURCE
  tests/main.test.hpp
  tests/toml.test.cpp
  tests/mods.test.cpp
  tests/packet.test.cpp
  tests/packet_handler.test.cpp
  )

## -------------------------------------------------------------------------- ##

set(CLIENT_SOURCE
  source/app/client/app_client.cpp
  source/app/client/imgui/imgui.cpp
  source/app/client/imgui/imgui_draw.cpp
  source/app/client/imgui/imgui_demo.cpp
  source/app/client/imgui/imgui_impl_glfw_gl3.cpp
  source/game/client/client_cache.cpp
  source/game/client/debug_ui.cpp
  source/game/client/game_client.cpp
  source/game/client/world_renderer.cpp
  source/graphics/camera.cpp
  source/graphics/index_buffer.cpp
  source/graphics/renderer.cpp
  source/graphics/shader.cpp
  source/graphics/sprite.cpp
  source/graphics/sprite_batch.cpp
  source/graphics/texture.cpp
  source/graphics/vertex_buffer.cpp
  )

## -------------------------------------------------------------------------- ##

set(SERVER_SOURCE
  source/app/server/app_server.cpp
  source/game/server/game_server.cpp
  source/game/server/cli_input.cpp
  )

## ========================================================================== ##
## Libraries
## ========================================================================== ##

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

# ENTT (ecs system)
include_directories(SYSTEM thirdparty/entt/single_include)
add_subdirectory(thirdparty/entt)

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
set(GLM_TEST_ENABLE OFF)
include_directories(SYSTEM thirdparty/glm)
add_subdirectory(thirdparty/glm)

# GLFW
include_directories(SYSTEM thirdparty/glfw/include)
add_subdirectory(thirdparty/glfw)

# Doctest
include_directories(SYSTEM thirdparty/doctest)
add_subdirectory(thirdparty/doctest)

# uuid
include_directories(SYSTEM thirdparty/stduuid/include)
add_subdirectory(thirdparty/stduuid)

# tessil robin hood map / set
include_directories(SYSTEM thirdparty/robin-map/include)

# Thidparty
include_directories(SYSTEM thirdparty)
aux_source_directory(thirdparty/glad CLIENT_SOURCE)

# binary output folder
set(out_dir ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)

## ========================================================================== ##
## Executable
## ========================================================================== ##

# Our executables / builds
add_executable(${PROJECT_NAME} source/main.cpp ${COMMON_SOURCE} ${CLIENT_SOURCE})
add_executable(server source/main.cpp ${COMMON_SOURCE} ${SERVER_SOURCE})
add_executable(test tests/main.test.cpp ${COMMON_SOURCE} ${TEST_SOURCE})

# Compile definitions?
if (CMAKE_BUILD_TYPE MATCHES "Debug")
  add_compile_definitions(DIB_DEBUG)
else ()
  add_compile_definitions(IMGUI_DISABLE_TEST_WINDOWS)
endif ()

add_compile_definitions(DLOG_TIMESTAMP DLOG_DATESTAMP)
#add_compile_definitions(DLOG_LOG_TO_FILE)
if (WIN32)
  add_compile_definitions(DIB_TARGET_WINDOWS)
else ()
  add_compile_definitions(DIB_TARGET_LINUX)
endif ()

target_compile_definitions(server PRIVATE DIB_IS_SERVER)

# Link libraries
if (WIN32)
  set(DIB_LIBS ws2_32 cpptoml dlog dutil glm gamenetworkingsockets
    alflibcpp ChakraCore EnTT uuid)
else ()
  set(DIB_LIBS pthread cpptoml dlog dutil glm GameNetworkingSockets alflibcpp ChakraCore dl m EnTT uuid)
endif ()

# TODO(Filip Björklund): Remove GLFW as a dependency on the server
target_link_libraries(${PROJECT_NAME} ${DIB_LIBS} glfw)
target_link_libraries(server ${DIB_LIBS})
target_link_libraries(test doctest ${DIB_LIBS} glfw)
