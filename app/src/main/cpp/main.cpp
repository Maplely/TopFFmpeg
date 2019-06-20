//
// Created by top on 2019/6/20.
//

#include "common.h"
JNIEXPORT void JNICALL Java_com_top_topffmpeg_activity_MainActivity_playVideo(JNIEnv *env, jobject obj, jstring path, jobject surface) {

    const char *path_str = env->GetStringUTFChars(path, 0);
    int res;

    av_register_all();

    AVFormatContext *fContext = avformat_alloc_context();

    res = avformat_open_input(&fContext, path_str, NULL, NULL);
    if (res != 0) {
        return;
    }
    res = avformat_find_stream_info(fContext, NULL);
    if (res < 0) {
        return;
    }

    int video_index = -1;

    for (int i = 0; i < fContext->nb_streams; i++) {

        if (fContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            break;
        }
    }
    LOGE_TTT("开始寻找解码器");
    //获得视频解码器
    AVCodecContext *video_co_context = fContext->streams[video_index]->codec;
    AVCodec *pCodec = avcodec_find_decoder(video_co_context->codec_id);
    if (pCodec == NULL) {
        LOGE_TTT("没有对应解码");
        return;
    }
    LOGE_TTT("开始解码");
    res=avcodec_open2(video_co_context,pCodec,NULL);
    if(res<0){
        LOGE_TTT("解码错误");
        return ;
    }
    
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    //todo
    AVPacket *pPacket = av_packet_alloc();
    
    //native窗体
    ANativeWindow *pWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时缓冲区
    ANativeWindow_Buffer outBuffer;
    int len,got_frame,framecount=0;
    LOGE_TTT("开始绘制窗口");
    while (av_read_frame(fContext,pPacket)>=0){
        len=avcodec_decode_video2(video_co_context,yuv_frame,&got_frame,pPacket);
        if(len>0 && got_frame){
            LOGE_TTT("解码第%d帧",framecount++);
            ANativeWindow_setBuffersGeometry(pWindow,video_co_context->width,video_co_context->height,WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(pWindow,&outBuffer,NULL);
            libyuv::I420ToARGB(
                    yuv_frame->data[0],yuv_frame->linesize[0],
                    yuv_frame->data[2],yuv_frame->linesize[2],
                    yuv_frame->data[1],yuv_frame->linesize[1],
                    rgb_frame->data[0],rgb_frame->linesize[0],
                    video_co_context->width,video_co_context->height
            );
            avpicture_fill((AVPicture *)rgb_frame,(uint8_t*)outBuffer.bits,AV_PIX_FMT_RGBA,video_co_context->width,video_co_context->height);
            ANativeWindow_unlockAndPost(pWindow);
            usleep(16*1000);
        }
        av_free_packet(pPacket);
    }
    
    av_frame_free(&yuv_frame);
    av_frame_free(&rgb_frame);
    avformat_free_context(fContext);
    avcodec_free_context(&video_co_context);

    env->ReleaseStringUTFChars(path, path_str);

}