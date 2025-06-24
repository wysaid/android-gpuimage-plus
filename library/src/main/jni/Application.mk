
# APP_ABI := armeabi-v7a arm64-v8a x86 
# armeabi & mips are deprecated
APP_ABI :=  all

APP_PLATFORM := android-19

APP_STL := c++_static

#APP_CPPFLAGS := -frtti -fexceptions
#APP_CPPFLAGS := -fpermissive
APP_CPPFLAGS := -frtti -std=c++14

APP_OPTIM := release

ifdef CGE_ENABLE_16KB_PAGE_SIZE
APP_SUPPORT_FLEXIBLE_PAGE_SIZES := true
endif