//
// Created by top on 2019/6/20.
//
#ifndef TOPFFMPEG_COMMON_H
#define TOPFFMPEG_COMMON_H
#define  MAX_AUDIO_FRME_SIZE 48000 * 4

#include <jni.h>
#include <android/log.h>
#include "unistd.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include <pthread.h>
#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR,TAG,##__VA_ARGS__)
#define LOGE_TTT(...) LOGE("TTT",##__VA_ARGS__)
#ifdef __cplusplus
extern "C" {
#endif

#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libyuv/convert_argb.h"
#include <libavutil/imgutils.h>
#include "include/libswresample/swresample.h"

JNIEXPORT void
JNICALL Java_com_top_topffmpeg_activity_MainActivity_playVideo(JNIEnv *env ,jobject obj,jstring path,jobject surface);

JNIEXPORT void JNICALL
Java_com_top_topffmpeg_activity_MainActivity_palySound(JNIEnv *env, jobject instance, jstring path_, jobject track);
#ifdef __cplusplus
};
#endif
#endif