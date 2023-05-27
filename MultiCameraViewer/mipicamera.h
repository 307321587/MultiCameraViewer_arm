#ifndef MIPICAMERA_H
#define MIPICAMERA_H
#include <iostream>
#include <QImage>
#include "opencv2/opencv.hpp"
#include "video_source.h"
#include "video_source_type.h"

#include <QObject>

using namespace std;
using namespace cv;

using videosource::VideoSource;
using videosource::VinModule;
using videosource::VideoSourceLogLevel;
using videosource::VideoSourceErrorCode;
using videosource::ImageFrame;
using videosource::PyramidFrame;
using videosource::HorizonVisionPixelFormat;

class MipiCamera:public QObject
{
    Q_OBJECT
public:
    explicit MipiCamera(QObject *parent = 0);
    ~MipiCamera();

    int Init(std::string path,int channel_id_); //failed return -1
    void startGrab();
    void stopGrab();
    QImage matToQImage(cv::Mat);
signals:
    void sendMatImage(QImage);
//    void sendMatImage();
private:
    std::string config_file;
    int channel_id;
    int ret;
    bool start_grab_flag;
    VideoSource* video_source;

};

#endif // MIPICAMERA_H
