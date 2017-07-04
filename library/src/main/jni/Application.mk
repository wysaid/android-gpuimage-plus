
APP_ABI :=  armeabi armeabi-v7a arm64-v8a x86 mips
#APP_ABI :=  armeabi-v7a

APP_PLATFORM := android-9

APP_STL := gnustl_static

#APP_CPPFLAGS := -frtti -fexceptions
#APP_CPPFLAGS := -fpermissive
APP_CPPFLAGS := -frtti -std=gnu++11

APP_OPTIM := release