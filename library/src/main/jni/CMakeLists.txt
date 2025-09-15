cmake_minimum_required(VERSION 3.19)

project(CGE)

set(CMAKE_CXX_STANDARD 14)

option(CGE_USE_VIDEO_MODULE "boolean option to enable video module" OFF)

if(NOT DEFINED ANDROID_GRADLE OR NOT ANDROID_GRADLE)
    if(NOT DEFINED ENV{NDK})
        execute_process(COMMAND which ndk-build OUTPUT_VARIABLE CGE_NDK_BUILD_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)

        if("${CGE_NDK_BUILD_PATH}" STREQUAL "")
            message(FATAL_ERROR "You should define the NDK variable!")
        else()
            get_filename_component(CGE_NDK_DIR "${CGE_NDK_BUILD_PATH}" DIRECTORY)
            set(ENV{NDK} "${CGE_NDK_DIR}")
            message(STATUS "Auto-detected NDK path: ${CGE_NDK_DIR}")
        endif()
    endif()

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

    if(DEFINED NDK_STANDALONE_TOOLCHAIN)
        set(NDK_INCLUDE_DIR ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include)

        if(EXISTS ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include/aarch64-linux-android)
            set(NDK_INCLUDE_DIR ${NDK_INCLUDE_DIR}
                ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include/aarch64-linux-android)
        endif()

        include_directories(${NDK_INCLUDE_DIR})
    endif()
endif()

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    execute_process(COMMAND cmd.exe /C "dir /L /S /B /A:D"
        OUTPUT_VARIABLE CGE_CORE_HEADER_DIR
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cge)
else()
    execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/cge -maxdepth 3 -type d OUTPUT_VARIABLE CGE_CORE_HEADER_DIR)
endif()

message(STATUS "CGE_CORE_HEADER_DIR=${CGE_CORE_HEADER_DIR}")
string(REGEX REPLACE "\n" ";" CGE_CORE_HEADER_DIR ${CGE_CORE_HEADER_DIR})

file(GLOB_RECURSE CGE_CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/cge/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cge/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/interface/*.cpp")

if(NOT DEFINED ANDROID_GRADLE OR NOT ANDROID_GRADLE)
    add_library(CGE ${CGE_CORE_SRC})
else()
    add_library(CGE SHARED ${CGE_CORE_SRC})
endif()

target_include_directories(CGE PUBLIC
    ${CGE_CORE_HEADER_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cge/common
    ${CMAKE_CURRENT_SOURCE_DIR}/cge/extends
    ${CMAKE_CURRENT_SOURCE_DIR}/cge/filters
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/custom)

if(NOT DEFINED ANDROID_ABI)
    target_compile_definitions(CGE PUBLIC ANDROID_ABI=arm64-v8a)
else()
    target_compile_definitions(CGE PUBLIC ANDROID_ABI=${ANDROID_ABI})
endif()

target_compile_definitions(CGE PUBLIC ANDROID_NDK=1 CGE_LOG_TAG=\"libCGE\" CGE_TEXTURE_PREMULTIPLIED=1 _CGE_DISABLE_GLOBALCONTEXT_=1 _CGE_ONLY_FILTERS_=1)

# 非 Debug 模式下添加 -O3
target_compile_options(CGE PRIVATE $<$<NOT:$<CONFIG:Debug>>:-O3>)

target_link_options(CGE PUBLIC -fPIE -fPIC -pie -Wl,-z,relro,-z,now)

target_link_libraries(CGE PUBLIC
    GLESv2
    EGL
    jnigraphics
    log
    android)

if(DEFINED ENABLE_16K_PAGE_SIZES AND ENABLE_16K_PAGE_SIZES)
    # 16KB elf:  <https://developer.android.com/guide/practices/page-sizes#cmake>
    target_link_options(CGE PUBLIC -Wl,-z,max-page-size=16384,-z,common-page-size=16384)
    target_compile_definitions(CGE PRIVATE ENABLE_16K_PAGE_SIZES=1)
    message("ENABLE_16K_PAGE_SIZES=${ENABLE_16K_PAGE_SIZES}")
endif()

# ########### Video module ############
if(CGE_USE_VIDEO_MODULE)
    add_library(ffmpeg SHARED IMPORTED)

    if(ENABLE_16K_PAGE_SIZES)
        set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-16kb")
    else()
        set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg")
    endif()

    target_include_directories(CGE PUBLIC ${FFMPEG_DIR}/include)
    target_compile_definitions(CGE PUBLIC CGE_USE_FFMPEG=1)

    set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libs/${ANDROID_ABI}/libffmpeg.so)

    target_link_libraries(CGE PUBLIC ffmpeg)
endif()

message("CGE_USE_VIDEO_MODULE=${CGE_USE_VIDEO_MODULE}")

# ########### ext module ############
file(GLOB_RECURSE CGE_EXT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/custom/*.cpp")
add_library(CGEExt SHARED ${CGE_EXT_SRC})

target_include_directories(CGEExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/custom)

target_link_libraries(CGEExt PRIVATE
    CGE)