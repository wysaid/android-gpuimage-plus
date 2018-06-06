set(CGEExt_ROOT ../source)
include_directories(
        ${CGEExt_ROOT}
        ${CGEExt_ROOT}/../include
        ${CGEExt_ROOT}/../include/filters
        ${CGEExt_ROOT}/../ffmpeg
        ${CGEExt_ROOT}/../interface
)

file(GLOB ext_cpp0 ${CGEExt_ROOT}/cgeVideoUtils.cpp)
file(GLOB ext_cpp1 ${CGEExt_ROOT}/cgeCustomFilters.cpp)
file(GLOB ext_cpp2 ${CGEExt_ROOT}/cgeCustomFilterWrapper.cpp)
file(GLOB ext_cpp3 ${CGEExt_ROOT}/customHelper.cpp)
file(GLOB ext_cpp4 ${CGEExt_ROOT}/customFilter_0.cpp)
file(GLOB ext_cpp5 ${CGEExt_ROOT}/customFilter_N.cpp)
add_library(CGEExt SHARED
${ext_cpp0}
${ext_cpp1}
${ext_cpp2}
${ext_cpp3}
${ext_cpp4}
${ext_cpp5}
)

add_library(libffmpeg SHARED IMPORTED)
set_target_properties( libffmpeg
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/libs/${ANDROID_ABI}/libffmpeg.so )

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


target_link_libraries(CGEExt
                     CGE
                     libffmpeg
                     android
                     ${log-lib}
                     ${egl-lib}
                     ${glesv2-lib}
                     ${graphics-lib}
                    )