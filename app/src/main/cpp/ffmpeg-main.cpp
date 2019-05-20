#include <jni.h>
#include "android/log.h"
#include "pthread.h"
#include "unistd.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "libyuv/convert_argb.h"

extern "C" {
#include "libavformat/avformat.h"

#define LOG_TAG "TTT"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,##__VA_ARGS__)


jobject pJobject;
jmethodID pID;
JavaVM *mVm;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    mVm = vm;
    LOGE("jni onload");
    return JNI_VERSION_1_4;
}

JNIEXPORT void
JNICALL
Java_com_top_topffmpeg_SecondActivity_playVideo(
        JNIEnv *env,
        jobject /* this */,
        jstring path,
        jobject surface) {

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
    LOGE("开始解析播放");
    //6一帧一帧读取压缩数据avpacket
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    //像素数据
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    //nativehuizhi
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_Buffer outBuffer;

    int len, got_frame, framecount = 0;

    //6读取数据
    while (av_read_frame(pContext, packet)>=0) {
        //解码AVPacket->AVFrame
        len = avcodec_decode_video2(pCodecContext, yuv_frame, &got_frame, packet);
        if (got_frame) {
            LOGE("解码%d帧", framecount++);

            ANativeWindow_setBuffersGeometry(nativeWindow, pCodecContext->width,
                                             pCodecContext->height, WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
            avpicture_fill((AVPicture *) rgb_frame, (uint8_t *) outBuffer.bits, AV_PIX_FMT_RGBA,
                           pCodecContext->width, pCodecContext->height);
            libyuv::I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                               yuv_frame->data[2], yuv_frame->linesize[2],
                               yuv_frame->data[1], yuv_frame->linesize[1],
                               rgb_frame->data[0], rgb_frame->linesize[0],
                               pCodecContext->width, pCodecContext->height);
            ANativeWindow_unlockAndPost(nativeWindow);
            usleep(1000 * 16);
        }
        av_free_packet(packet);
    }
    ANativeWindow_release(nativeWindow);
    av_frame_free(&yuv_frame);
    avcodec_close(pCodecContext);
    avformat_free_context(pContext);
    env->ReleaseStringUTFChars(path, sPath);


}


JNIEXPORT void JNICALL
Java_com_top_topffmpeg_PosixThread_posix_1init(JNIEnv *env, jclass type) {
    jclass pJclass = env->FindClass("com/top/topffmpeg/UUIDUtils");
    //create global refrence
    pJobject = env->NewGlobalRef(pJclass);
    pID = env->GetStaticMethodID((jclass) pJobject, "generatUUid", "()Ljava/lang/String;");

}

JNIEXPORT void JNICALL
Java_com_top_topffmpeg_PosixThread_posix_1destroy(JNIEnv *env, jclass type) {
    env->DeleteGlobalRef(pJobject);
}

void *th_fun(void *arg) {
    for (int i = 0; i < 5; ++i) {
        JNIEnv *env;
        mVm->AttachCurrentThread(&env, NULL);
        jobject uuid = env->CallStaticObjectMethod((jclass) pJobject, pID);
        const char *string = env->GetStringUTFChars(static_cast<jstring>(uuid), 0);
        LOGE("<jni>得到的uuid为:%s", string);
        if (i == 4) {
            goto end;
        }
        sleep(1);
    }
    end:
    mVm->DetachCurrentThread();
    pthread_exit(0);
    LOGE("posix退出");
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGE("jni退出");
}


JNIEXPORT void JNICALL
Java_com_top_topffmpeg_PosixThread_posix_1getuuid(JNIEnv *env, jclass type) {
    pthread_t tid;
    pthread_create(&tid, NULL, th_fun, NULL);

}
}
