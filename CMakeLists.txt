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
        source/game.cpp
        source/app/app.cpp
        source/app/graphics/graphics.cpp
        source/script/env.cpp
        source/script/util.cpp
        source/script/script.cpp
        source/script/expose/expose_base.cpp
        source/script/expose/expose_mod.cpp
        source/mods/loader.cpp
        source/mods/loader.hpp
        source/mods/mod_info.hpp
        )

# ChakraCore
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_SOURCE_DIR}/lib/chakracore/lib/debug)
else ()
    link_directories(${CMAKE_SOURCE_DIR}/lib/chakracore/lib/release)
endif ()

set(TEST_SOURCE
        tests/main.test.hpp
        tests/toml.test.cpp
        )

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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)

# Our executables / builds
add_executable(${PROJECT_NAME} source/main.cpp ${COMMON_SOURCE})
add_executable(test tests/main.test.cpp ${COMMON_SOURCE} ${TEST_SOURCE})

# Compile definitions?
add_compile_definitions(${PROJECT_NAME} DIB_DEBUG)
add_compile_definitions(${PROJECT_NAME} SDL_MAIN_HANDLED)
add_compile_definitions(${PROJECT_NAME} DLOG_TIMESTAMP DLOG_DATESTAMP)
#add_compile_definitions(${PROJECT_NAME} DLOG_LOG_TO_FILE)
if (WIN32)
    add_compile_definitions(${PROJECT_NAME} DIB_TARGET_WINDOWS)
else ()
    add_compile_definitions(${PROJECT_NAME} DIB_TARGET_LINUX)
endif ()

# Link libraries
if (WIN32)
    set(DIB_LIBS ws2_32 cpptoml dlog dutil glfw alflibcpp ChakraCore)
else ()
    set(DIB_LIBS pthread cpptoml dlog dutil glfw alflibcpp ChakraCore)
endif ()

target_link_libraries(${PROJECT_NAME} ${DIB_LIBS})
target_link_libraries(test doctest ${DIB_LIBS})