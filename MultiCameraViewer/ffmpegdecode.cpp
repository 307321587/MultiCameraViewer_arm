//#pragma execution_character_set(""utf-8"")

#include "ffmpegdecode.h"
FFmpegDecode::FFmpegDecode(const char *url,MyMediaType media_type):
    isOpen(false)
{
    //RAW_LOG(ERROR, "Failed foo with %i: %s", status, error);
    // 变量初始化
    pFormat_Ctx_=NULL; // 媒体流编码上下文
    video_index_=0;  // 视频索引号
    pCodec_Ctx_=NULL;
    pCodec_=NULL;
    pFrame_=NULL;
    pFrame_RGB_=NULL;
    packet_=NULL;
    img_convert_ctx_=NULL;

    is_preview_ =true;
    media_type_=media_type;

    // 定义流描述头
    pFormat_Ctx_ = avformat_alloc_context();

    //输入流为视频
    if(!media_type_)
    {
        // 根据url打开流     
        if (avformat_open_input(&pFormat_Ctx_, url, NULL, NULL) != 0){
            qWarning("无法打开视频输入流");
            return ;
        }
    }
    //输入流为相机
    else
    {
        avdevice_register_all();
        const AVInputFormat *ifmt = av_find_input_format("dshow");
        if (avformat_open_input(&pFormat_Ctx_, url, ifmt, NULL) != 0)
        {
            isOpen = false;
            qWarning("无法打开相机输入流");
           return ;
        }
    }

    // 获取音视频流信息
    if (avformat_find_stream_info(pFormat_Ctx_, NULL) < 0){
        qWarning("无法找到流信息");
        return ;
    }

    video_index_ = -1;

    //nb_streams视音频流的个数，这里当查找到视频流时就中断了。
    for (int i = 0; i < pFormat_Ctx_->nb_streams; i++)
    {
        if (pFormat_Ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index_ = i;
            break;
        }
    }
    if (video_index_ == -1){
        qWarning("无法找到视频编码流");
        return ;
    }

    // 获取编码器
    pCodec_ = avcodec_find_decoder(pFormat_Ctx_->streams[video_index_]->codecpar->codec_id);

    // 获取视频流编码结构
    pCodec_Ctx_ = avcodec_alloc_context3(pCodec_);
    avcodec_parameters_to_context(pCodec_Ctx_, pFormat_Ctx_->streams[video_index_]->codecpar);
    if (pCodec_ == NULL){
        qWarning("无法找到解码器\n");
        return ;
    }

    //用于初始化pCodecCtx结构
    if (avcodec_open2(pCodec_Ctx_, pCodec_, NULL) < 0){
        qWarning("无法打开解码器\n");
        return ;
    }

    //定义完成解码的frame
    pFrame_ = av_frame_alloc();
    pFrame_RGB_ = av_frame_alloc();

    // 创建动态内存,创建存储图像数据的空间
    // av_image_get_buffer_size获取一帧图像需要的大小
    // av_image_fill_arrays关联out_buffer创建的空间,该空间用于存放frame数据的内存地址
    unsigned char *out_buffer;
    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32 , pCodec_Ctx_->width, pCodec_Ctx_->height, 1));
    av_image_fill_arrays(pFrame_RGB_->data, pFrame_RGB_->linesize, out_buffer,AV_PIX_FMT_RGB32, pCodec_Ctx_->width, pCodec_Ctx_->height, 1);

    //定义未解码的packet
    packet_ = (AVPacket *)av_malloc(sizeof(AVPacket));

    //初始化img_convert_ctx结构 将原视频流转化为ARGB4444
    img_convert_ctx_ = sws_getContext(pCodec_Ctx_->width, pCodec_Ctx_->height, pCodec_Ctx_->pix_fmt,
                                      pCodec_Ctx_->width, pCodec_Ctx_->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    qInfo("Init finsh");
    isOpen = true;
}

FFmpegDecode::~FFmpegDecode()
{
    FreeRam();
    qInfo("解码器释放");
}



void FFmpegDecode::DecodeMedia()
{
    int ret;
    while(is_preview_)
    {
        if (av_read_frame(pFormat_Ctx_, packet_) >= 0)
        {
            //如果是视频数据
            if (packet_->stream_index == video_index_)
            {
                //解码一帧视频数据
                if (avcodec_send_packet(pCodec_Ctx_, packet_))
                    break;
                ret = avcodec_receive_frame(pCodec_Ctx_, pFrame_);

                if (ret == AVERROR_EOF) {
                    printf("Decode Error.\n");
                    return;
                }
                if (ret == 0) {
                    sws_scale(img_convert_ctx_, (const unsigned char* const*)pFrame_->data, pFrame_->linesize, 0, pCodec_Ctx_->height,
                        pFrame_RGB_->data, pFrame_RGB_->linesize);
                    QImage img((uchar*)pFrame_RGB_->data[0], pCodec_Ctx_->width, pCodec_Ctx_->height, QImage::Format_RGB32);//此处是传输ARGB4444通道数据

                    emit SendVideoBuffer(img, media_type_);
//                    qDebug()<<"ffmpeg decode: "<< QThread::currentThread();

                    QThread::msleep(1);// 线程阻塞休眠
                }
            }

            av_packet_unref(packet_);
            av_freep(packet_);
            QThread::msleep(1);
        }
    }

}

void FFmpegDecode::Delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void FFmpegDecode::FreeRam()
{
    // 释放图像转换
    if (img_convert_ctx_ != NULL) {
        sws_freeContext(img_convert_ctx_);
        img_convert_ctx_ = NULL;
        qInfo("释放图像转换");
    }

    // 释放流中取出的包
    if (packet_ != NULL) {
        av_packet_unref(packet_);
        packet_ = NULL;
        qInfo("释放流中取出的包");
    }

    // 释放解码后的帧
    if (pFrame_ != NULL) {
        av_frame_free(&pFrame_);
        pFrame_ = NULL;
        qInfo("释放解码后的帧");
    }

    // 释放解码器
    if (pCodec_Ctx_ != NULL) {
        avcodec_close(pCodec_Ctx_);
        pCodec_ = NULL;
        qInfo("释放解码器");
    }

    // 释放流信息头
    if (pFormat_Ctx_ != NULL) {
        avformat_close_input(&pFormat_Ctx_);
        pFormat_Ctx_ = NULL;
        qInfo("释放流信息头");
    }


}

void FFmpegDecode::PreviewStop()
{
    mutex_.lock();
    is_preview_ = false;
    mutex_.unlock();

}



