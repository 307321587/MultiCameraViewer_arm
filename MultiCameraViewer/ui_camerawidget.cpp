//#include "ui_camerawidget.h"
#include "ui_camerawidget.h"
#include "ui_ui_camera_viewer.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QImage>
#include <QList>
#include <QMessageBox>

CameraWidget::CameraWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraWidget)
{
    ui->setupUi(this);
    enum_camera();

    for(int i=0;i<2;i++)

    {
        ff_decode_[i]=NULL;
    }

    ui->btn_close_camera->setEnabled(false);
    connect(ui->btn_open_camera,SIGNAL(clicked(bool)),this,SLOT(btn_camera_open()));
    connect(ui->btn_start_grab,SIGNAL(clicked(bool)),this,SLOT(btn_start_grab()));
    connect(ui->btn_close_camera,SIGNAL(clicked(bool)),this,SLOT(btn_camera_close()));
    connect(ui->btn_stop_grab,SIGNAL(clicked(bool)),this,SLOT(btn_stop_grab()));
    connect(ui->btn_clockwise_90,SIGNAL(clicked(bool)),this,SLOT(btn_clockwise_rotate()));
    connect(ui->btn_counterclockwise_90,SIGNAL(clicked(bool)),this,SLOT(btn_counterclockwise_rotate()));
    connect(ui->btn_image_shoot,SIGNAL(clicked(bool)),this,SLOT(btn_image_shoot()));
    connect(ui->btn_horizontal_flip,SIGNAL(clicked(bool)),this,SLOT(btn_horizontal_flip()));
    connect(ui->btn_vertical_flip,SIGNAL(clicked(bool)),this,SLOT(btn_vertical_flip()));
    connect(ui->img_view,&InteractiveView::send_RGBVal,this,[=](int R,int G,int B){
        ui->color_label->setText(QString("RGB(%1,%2,%3)").arg(R).arg(G).arg(B));
    });
}


void CameraWidget::btn_camera_open()
{
    if(!ff_decode_[1]){
        QString camera_name = cameras_name[ui->camera_list->currentIndex()];
//        QString camera_name="/dev"+ui->camera_list->currentText();
        std::string str = camera_name.toStdString();
        const char* camera_name_str = str.c_str();
        ff_decode_[1] =new FFmpegDecode(camera_name_str,MYMEDIA_CAMERA);
        if(!ff_decode_[1]->isOpen)
        {
            QMessageBox::information(NULL,"ERROR!",QString("camera cannot open!"));
            ff_decode_[1]=NULL;
            return;
        }
        else
        {
            ff_decode_[1]->moveToThread(t1);
            connect(t1,&QThread::started,ff_decode_[1],&FFmpegDecode::DecodeMedia);
            t1->start();
            ui->btn_open_camera->setEnabled(false);
            ui->btn_close_camera->setEnabled(true);
        }
    }
}

void CameraWidget::btn_start_grab()
{

    connect(ff_decode_[1], &FFmpegDecode::SendVideoBuffer, this,&CameraWidget::show_img);

}

void CameraWidget::btn_camera_close()
{
    if(ff_decode_[1])
    {
        ff_decode_[1]->PreviewStop();
        t1->quit();
        t1->wait(500);
        delete ff_decode_[1];
        btn_stop_grab();
        ff_decode_[1] = NULL;
        this->ui->img_view->menu_clean();
        ui->btn_open_camera->setEnabled(true);
        ui->btn_close_camera->setEnabled(false);
    }
}

void CameraWidget::btn_stop_grab()
{
    disconnect(ff_decode_[1], &FFmpegDecode::SendVideoBuffer, this,&CameraWidget::show_img);
}

void CameraWidget::btn_horizontal_flip()
{
    if(!ui->img_view->is_horizontal())
        ui->img_view->set_mirrored(true,false);
    else
        ui->img_view->set_mirrored(false,false);
}

void CameraWidget::btn_vertical_flip()
{
    if(!ui->img_view->is_vertical())
        ui->img_view->set_mirrored(false,true);
    else
        ui->img_view->set_mirrored(false,false);
}

void CameraWidget::btn_clockwise_rotate()
{
    ui->img_view->rotate(90);
}

void CameraWidget::btn_counterclockwise_rotate()
{
    ui->img_view->rotate(-90);
}

void CameraWidget::btn_cam_parameter_set()
{
//    QMessageBox(this,"设置相机参数",);
    QMessageBox *ms = new QMessageBox(this);
    ms->resize(600,400);
    ms->setMinimumSize(QSize(600,400));
    ms->setMaximumSize(QSize(600,400));
//    ms->setDefaultButton(p1);
    ms->show();

}

void CameraWidget::btn_image_shoot()
{
    ui->img_view->menu_save();
}

void CameraWidget::enum_camera()
{
    const std::string dev_folder = "/dev/"; // 设备所在目录
    DIR *dir;
    struct dirent *ent; //用来保存 /dev下的子目录
    if((dir = opendir(dev_folder.c_str())) != NULL) //如果能够正常打开
    {
        while ((ent = readdir(dir)) != NULL ) //读取 /dev下的子目录
        {
            if(!strncmp(ent->d_name,"video",5)) //通过名称筛选 video 类型设备
            {

                std::string file = dev_folder + ent->d_name; //获取 /dev/video*
                const int fd = open(file.c_str(),O_RDWR,0);
                struct v4l2_capability cap;
                if (fd >= 0 )
                {
                    ioctl(fd,VIDIOC_QUERYCAP,&cap);
                    char *str =(char *)cap.card;
                    QString cap_name = QString(QLatin1String(str));
                    this->ui->camera_list->addItem(cap_name);
                    cameras_name.push_back(QString::fromStdString(file));
                }
            }
        }
        closedir(dir);
    }
}

void CameraWidget::show_img(QImage video_buffer, int media_type)
{
    Q_UNUSED(media_type);
    this->ui->img_view->show_Qimage(video_buffer);
}

void CameraWidget::camera_update()
{
    ui->camera_list->clear();
//    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();   //相机枚举
//    foreach (const QCameraInfo &cameraInfo, cameras){
//        this->ui->camera_list->addItem(cameraInfo.description());
//        cameras_name.push_back(cameraInfo.deviceName());
//    }
    enum_camera();
}

CameraWidget::~CameraWidget()
{
    delete ui;

}
