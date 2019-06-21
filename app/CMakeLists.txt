cmake_minimum_required(VERSION 3.4.1)

add_library(ffmpeg-main SHARED src/main/cpp/main.cpp src/main/cpp/common.h src/main/cpp/sound_paly.cpp)

target_include_directories(ffmpeg-main PRIVATE ${CMAKE_SOURCE_DIR}/src/main/cpp/include) #头文件

add_library(fame SHARED IMPORTED)
set_target_properties(fame PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libffmpeg.so)

add_library(yuv SHARED IMPORTED)
set_target_properties(yuv PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libyuv.so)

find_library(log-lib log)

target_link_libraries(ffmpeg-main fame  yuv ${log-lib} android)