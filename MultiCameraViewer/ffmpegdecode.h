# pragma once

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
}

#include <stdio.h>
#include <iostream>

#include <QObject>
#include <QImage>
#include <QTime>
#include <QCoreApplication>
#include <qthread.h>
#include <QDebug>
#include <QMutex>




enum MyMediaType{
  MYMEDIA_VIDEO=0,
  MYMEDIA_CAMERA=1
};

class FFmpegDecode:public QObject
{
    Q_OBJECT
public:
    FFmpegDecode(const char*url, MyMediaType media_type);
    ~FFmpegDecode();
    void DecodeMedia();
    void Delay(int msec);
    void FreeRam();
    void PreviewStop();

    bool isOpen;


private:
    AVFormatContext	*pFormat_Ctx_; // 媒体流编码上下文
    int				video_index_;  // 视频索引号
    AVCodecContext	*pCodec_Ctx_;
    const AVCodec			* pCodec_;
    AVFrame    *pFrame_, *pFrame_RGB_;
    AVPacket *packet_;
    struct SwsContext *img_convert_ctx_;

    bool is_preview_;
    MyMediaType media_type_;
    QMutex mutex_;

signals:
    void SendVideoBuffer(QImage video_buffer,int media_type);



};



