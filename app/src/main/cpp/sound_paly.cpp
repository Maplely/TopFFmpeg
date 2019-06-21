//
// Created by top on 2019/6/20.
//
#include "common.h"


JNIEXPORT void JNICALL
Java_com_top_topffmpeg_activity_MainActivity_palySound(JNIEnv *env, jobject instance, jstring path_, jobject track) {
    const char *path = env->GetStringUTFChars(path_, 0);
    av_register_all();
    AVFormatContext *fContext = avformat_alloc_context();
    if (avformat_open_input(&fContext, path, NULL, NULL) != 0) {
        LOGE_TTT("打开文件错误");
        return;
    }
    if (avformat_find_stream_info(fContext, NULL) < 0) {
        LOGE_TTT("寻找信息错误");
        return;
    }
    int audioInex = -1;
    for (int i = 0; i < fContext->nb_streams; i++) {
        if (fContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioInex = i;
        }
    }
    if (audioInex == -1) {
        LOGE_TTT("没有找到音频");
        return;
    }
    AVCodecContext *codecContext = fContext->streams[audioInex]->codec;
    AVCodec *pCodec = avcodec_find_decoder(codecContext->codec_id);
    if (pCodec == NULL) {
        LOGE_TTT("没有对应解码方法");
        return;
    }
    if (avcodec_open2(codecContext, pCodec, NULL) < 0) {
        LOGE_TTT("解码失败");
        return;
    }
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();
    SwrContext *swrAlloc = swr_alloc();
    //`````````设置重采样参数  
    uint64_t in_ch_layout = codecContext->channel_layout;
    int out_ch_layout = AV_CH_LAYOUT_STEREO;

    AVSampleFormat in_format = codecContext->sample_fmt;
    AVSampleFormat out_sampleFormat = AV_SAMPLE_FMT_S16;
    uint8_t *out_buffer = static_cast<uint8_t *>(av_malloc(MAX_AUDIO_FRME_SIZE));
    int in_rate = codecContext->sample_rate;
    int out_rate = in_rate;
    //~~~~~~~~~结束
    swr_alloc_set_opts(swrAlloc, out_ch_layout, out_sampleFormat, out_rate,
                       in_ch_layout, in_format, in_rate,
                       0, NULL);
    swr_init(swrAlloc);

    int channels = av_get_channel_layout_nb_channels(out_ch_layout);
    int got_frame, index = 0, res;
    //~~~~~~~~jni start

    jclass audioTrackClass = env->GetObjectClass(track);
    jmethodID trackMethodId = env->GetMethodID(audioTrackClass, "write", "([BII)I");
    jmethodID playMehodId = env->GetMethodID(audioTrackClass, "play", "()V");
    env->CallVoidMethod(track,playMehodId);
    //~~~~~~~~jni end

    while (av_read_frame(fContext, pPacket) >= 0) {
        //解码音频类型
        if (pPacket->stream_index == audioInex) {
            res = avcodec_decode_audio4(codecContext, pFrame, &got_frame, pPacket);
            if (res < 0) {
                LOGE_TTT("解码完成");
            }
            if (got_frame > 0) {
                LOGE_TTT("音频解码%d帧", index++);
                swr_convert(swrAlloc, &out_buffer, MAX_AUDIO_FRME_SIZE, (const uint8_t **) pFrame->data,
                            pFrame->nb_samples);
                int out_buffer_size = av_samples_get_buffer_size(NULL, channels, pFrame->nb_samples, out_sampleFormat,
                                                                 1);

                jbyteArray sample_array = env->NewByteArray(out_buffer_size);
                jbyte *bytes = env->GetByteArrayElements(sample_array, 0);
                memcpy(bytes, out_buffer, out_buffer_size);
                env->ReleaseByteArrayElements(sample_array, bytes, 0);

                env->CallIntMethod(track, trackMethodId, sample_array, 0, out_buffer_size);
                env->DeleteLocalRef(sample_array);

                usleep(16 * 1000);
            }

        }
        av_free_packet(pPacket);
    }
    av_frame_free(&pFrame);
    av_free(out_buffer);
    swr_free(&swrAlloc);
    avcodec_close(codecContext);
    avformat_close_input(&fContext);
    env->ReleaseStringUTFChars(path_, path);
}
