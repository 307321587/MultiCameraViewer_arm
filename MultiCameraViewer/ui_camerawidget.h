#ifndef UI_CAMERAWIDGET_H
#define UI_CAMERAWIDGET_H

#include <QWidget>
//#include <image_process.h>
#include <QPixmap>
#include <QVector>
#include <QThread>
//#include <QCameraInfo>
//#include <opencv2/opencv.hpp>
#include "drawwidget.h"
#include "ffmpegdecode.h"

#include <fcntl.h>  // 打开文件 open()
#include <unistd.h> //  关闭文件 close()
#include <sys/ioctl.h> //设备内核操作
#include <sys/mman.h>   //映射函数
#include <linux/videodev2.h>    //设备结构体定义
#include <dirent.h> // 列出当前目录下的所有内容
#include <linux/media.h>

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

namespace Ui {
class CameraWidget;
}

class CameraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWidget(QWidget *parent = 0);
    ~CameraWidget();
//    void image_show(QImage qimage);

public slots:
    void btn_camera_open();
    void btn_start_grab();
    void btn_camera_close();
    void btn_stop_grab();
    void btn_horizontal_flip();
    void btn_vertical_flip();
    void btn_clockwise_rotate();
    void btn_counterclockwise_rotate();
    void btn_cam_parameter_set();
    void btn_image_shoot();

    void enum_camera();

private:
    Ui::CameraWidget *ui;
    int count_img_ = 0;
    QVector<QString> cameras_name;

public:
    QThread *t1 = new QThread;
    FFmpegDecode* ff_decode_[2];
    void show_img(QImage video_buffer,int media_type);
    void camera_update();

};

#endif // UI_CAMERAWIDGET_H
