set(lib_DIR ../../../../src/main/libs/${ANDROID_ABI})
set(CGE_ROOT ../)
set(CGE_ROOT_C ${CGE_ROOT}/cge)
set(CGE_INCLUDE ${CGE_ROOT}/include)
set(CGE_FFMPEG ${CGE_ROOT}/ffmpeg)
set(CGE_COMMON ${CGE_ROOT_C}/common)
set(CGE_EXTENDS ${CGE_ROOT_C}/extends)
set(CGE_FILTERS ${CGE_ROOT_C}/filters)
set(CGE_INTERFACE ${CGE_ROOT}/interface)

include_directories(
        ${CGE_INCLUDE}
        ${CGE_ROOT}/interface
        ${CGE_INCLUDE}/filters
        ${CGE_ROOT}/ffmpeg
        ${CGE_ROOT_C}
)

add_library(libffmpeg SHARED IMPORTED)
set_target_properties( libffmpeg
                       PROPERTIES IMPORTED_LOCATION
                       ${lib_DIR}/libffmpeg.so )
file(GLOB common_src ${CGE_COMMON}/*.cpp)
file(GLOB extends ${CGE_EXTENDS}/*.cpp)
file(GLOB filters ${CGE_FILTERS}/*.cpp)
file(GLOB interface ${CGE_INTERFACE}/*.cpp)
add_library(CGE SHARED
            ${common_src}
            ${extends}
            ${filters}
            ${interface}
            )
find_library(log-lib
             log
             )
find_library(egl-lib
             EGL
             )
find_library(glesv2-lib
             GLESv2
             )
find_library(graphics-lib
             jnigraphics
             )
find_library(atomiclib
              atomic
              )
target_link_libraries(CGE
                     libffmpeg
                     android
                     ${log-lib}
                     ${egl-lib}
                     ${glesv2-lib}
                     ${graphics-lib}
                     ${atomic}
                    )
