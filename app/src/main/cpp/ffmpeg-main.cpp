#include <jni.h>
#include "android/log.h"

extern "C" {
#include "libavformat/avformat.h"
}

#define LOG_TAG "TTT"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,##__VA_ARGS__)

extern "C" JNIEXPORT void
JNICALL
Java_com_top_topffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jstring path) {

    const char *sPath = env->GetStringUTFChars(path, 0);
    //1.注册
    av_register_all();
    //2.拿到上下文
    AVFormatContext *pContext = avformat_alloc_context();
    //3.打开视频文件
    if (avformat_open_input(&pContext, sPath, NULL, NULL) != 0) {
        LOGE("%s", "打开视频失败!");
        return;
    }
    if (avformat_find_stream_info(pContext, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;
    }
    int video_index = -1;
    int i = 0;
    for (; i < pContext->nb_streams; i++) {
        if (pContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            break;
        }
    }
    //4.解码器  解码上下文
    AVCodecContext *pCodecContext = pContext->streams[video_index]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodecContext->codec_id);
    if (pCodec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }
    //5打开解码器
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }
    //6一帧一帧读取压缩数据avpacket
    AVPacket *avPacket;
    av_init_packet(avPacket);
    //像素数据
    AVFrame *pFrame;
    pFrame = av_frame_alloc();
    int got_frame;
    while (av_read_frame(pContext, avPacket) >= 0) {
        avcodec_decode_video2(pCodecContext, pFrame, &got_frame, avPacket);
    }
    

    env->ReleaseStringUTFChars(path, sPath);


}
