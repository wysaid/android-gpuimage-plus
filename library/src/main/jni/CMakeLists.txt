cmake_minimum_required(VERSION 3.9)

project(CGE)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -frtti -std=c++14")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")

if(NOT DEFINED ENV{NDK})
    execute_process(COMMAND which ndk-build OUTPUT_VARIABLE CGE_NDK_DIR)
    if("${CGE_NDK_DIR}" STREQUAL "")
        message(FATAL_ERROR "You should define the NDK variable!")
    else()
        execute_process(COMMAND export NDK=$(dirname `which ndk-build`))
    endif()
endif()

execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/setup_android_toolchain)

set(NDK_STANDALONE_TOOLCHAIN $ENV{NDK}/ndk-build-toolchain)
message(${NDK_STANDALONE_TOOLCHAIN})
set(CMAKE_SYSTEM_NAME AndroidARM)
set(CMAKE_C_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang)
set(CMAKE_CXX_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang++)
set(CMAKE_FIND_ROOT_PATH ${NDK_STANDALONE_TOOLCHAIN})

execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/cge -maxdepth 3 -type d OUTPUT_VARIABLE CGE_CORE_HEADER_DIR)
execute_process(COMMAND bash -c "find ${CMAKE_CURRENT_SOURCE_DIR} -type f -iname '*.cpp' | grep -v cmake" OUTPUT_VARIABLE CGE_CORE_SRC)

string(REGEX REPLACE "\n" ";" CGE_CORE_HEADER_DIR ${CGE_CORE_HEADER_DIR})
string(REGEX REPLACE "\n" ";" CGE_CORE_SRC ${CGE_CORE_SRC})

include_directories(
        ${CGE_CORE_HEADER_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/filters
        ${CMAKE_CURRENT_SOURCE_DIR}/interface
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/faceTracker
        ${CMAKE_CURRENT_SOURCE_DIR}/faceTracker/FaceTracker
        ${CMAKE_CURRENT_SOURCE_DIR}/opencv
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg
        ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include)

add_definitions(-DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D_CGE_USE_FFMPEG_ -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_ -DANDROID_ARM_NEON -DANDROID_ABI=armeabi-v7a --sysroot=${NDK_STANDALONE_TOOLCHAIN}/sysroot)


# file(GLOB_RECURSE FM_CORE_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/facemagic/*.cpp")

add_library(CGE
        ${CGE_CORE_SRC}
        )


target_link_libraries(CGE)