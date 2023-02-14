cmake_minimum_required(VERSION 3.19)

project(CGE)

set(CMAKE_CXX_STANDARD 14)

option(CGE_USE_VIDEO_MODULE "boolean option to enable video module" OFF)

if (NOT DEFINED ANDROID_GRADLE OR NOT ANDROID_GRADLE)
    if (NOT DEFINED ENV{NDK})
        execute_process(COMMAND which ndk-build OUTPUT_VARIABLE CGE_NDK_DIR)
        if ("${CGE_NDK_DIR}" STREQUAL "")
            message(FATAL_ERROR "You should define the NDK variable!")
        else ()
            execute_process(COMMAND export NDK=$ (dirname `which ndk-build`))
        endif ()
    endif ()

    execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/setup_android_toolchain)

    set(NDK_STANDALONE_TOOLCHAIN $ENV{NDK}/ndk-build-toolchain)
    message(${NDK_STANDALONE_TOOLCHAIN})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -frtti -std=c++14")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE")
    set(CMAKE_C_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang)
    set(CMAKE_CXX_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang++)
    set(CMAKE_SYSROOT ${NDK_STANDALONE_TOOLCHAIN}/sysroot)
    set(CMAKE_FIND_ROOT_PATH ${NDK_STANDALONE_TOOLCHAIN})

    if (DEFINED NDK_STANDALONE_TOOLCHAIN)
        set(NDK_INCLUDE_DIR ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include)
        if (EXISTS ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include/aarch64-linux-android)
            set(NDK_INCLUDE_DIR ${NDK_INCLUDE_DIR}
                    ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include/aarch64-linux-android)
        endif ()
        include_directories(${NDK_INCLUDE_DIR})
    endif ()
endif ()

if (NOT DEFINED ANDROID_ABI)
    add_definitions(-DANDROID_ABI=arm64-v8a)
else ()
    add_definitions(-DANDROID_ABI=${ANDROID_ABI})
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs/${ANDROID_ABI}")
endif ()

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    execute_process(COMMAND cmd.exe /C "dir /L /S /B /A:D"
            OUTPUT_VARIABLE CGE_CORE_HEADER_DIR
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cge)
else ()
    execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/cge -maxdepth 3 -type d OUTPUT_VARIABLE CGE_CORE_HEADER_DIR)
endif ()

message("CGE_CORE_HEADER_DIR=${CGE_CORE_HEADER_DIR}")
string(REGEX REPLACE "\n" ";" CGE_CORE_HEADER_DIR ${CGE_CORE_HEADER_DIR})

# execute_process(COMMAND bash -c "find \"${CMAKE_CURRENT_SOURCE_DIR}/cge\" -type f -iname '*.cpp' -o -iname '*.c'" OUTPUT_VARIABLE CGE_CORE_SRC)
# string(REGEX REPLACE "\n" ";" CGE_CORE_SRC ${CGE_CORE_SRC})

file(GLOB_RECURSE CGE_CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/cge/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/cge/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/interface/*.cpp")

include_directories(
        ${CGE_CORE_HEADER_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cge/common
        ${CMAKE_CURRENT_SOURCE_DIR}/cge/extends
        ${CMAKE_CURRENT_SOURCE_DIR}/cge/filters
        ${CMAKE_CURRENT_SOURCE_DIR}/interface
        ${CMAKE_CURRENT_SOURCE_DIR}/custom
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg)

add_definitions(-DANDROID_NDK=1 -DCGE_LOG_TAG=\"libCGE\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -D_CGE_ONLY_FILTERS_=1)

if (CGE_USE_VIDEO_MODULE)

    # ffmpeg ref
    add_library(ffmpeg SHARED IMPORTED)
    set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION
            ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/${ANDROID_ABI}/libffmpeg.so)

    set(CGE_VIDEO_MODULE "ffmpeg")
    add_definitions(-DCGE_USE_FFMPEG=1)
    # if (DEFINED ANDROID_ABI)
    #     configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/${ANDROID_ABI}/libffmpeg.so 
    #               ${CMAKE_CURRENT_SOURCE_DIR}/../libs/${ANDROID_ABI}/libffmpeg.so COPYONLY)
    #     # set_target_properties(ffmpeg PROPERTIES
    #     #         LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../libs/${ANDROID_ABI})
    #     # set_target_properties(ffmpeg PROPERTIES
    #     #         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../libs/${ANDROID_ABI})
    # endif ()
endif ()

message("CGE_USE_VIDEO_MODULE=${CGE_USE_VIDEO_MODULE}")

if (NOT DEFINED ANDROID_GRADLE OR NOT ANDROID_GRADLE)
    add_library(CGE
            ${CGE_CORE_SRC}
            )
else ()
    add_library(CGE SHARED
            ${CGE_CORE_SRC}
            )
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/custom)
file(GLOB_RECURSE CGE_EXT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/custom/*.cpp")

add_library(CGEExt SHARED ${CGE_EXT_SRC})

# if (DEFINED ANDROID_ABI)
#     set_target_properties(CGE PROPERTIES
#             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../libs/${ANDROID_ABI})
#     set_target_properties(CGEExt PROPERTIES
#             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../libs/${ANDROID_ABI})
# endif ()

target_link_libraries(CGE
        ${CGE_VIDEO_MODULE}
        GLESv2
        EGL
        jnigraphics
        log
        android
        )

target_link_libraries(CGEExt
        CGE
        ${CGE_VIDEO_MODULE}
        GLESv2
        EGL
        jnigraphics
        log
        android)