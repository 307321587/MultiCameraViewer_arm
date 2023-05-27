#include "mipicamera.h"
#include <QThread>
MipiCamera::MipiCamera(QObject *parent)
        :QObject(parent)
{

}

MipiCamera::~MipiCamera()
{

}

int MipiCamera::Init(string path, int channel_id_)
{
   ret = 0;
   config_file = path;
   channel_id = channel_id_;
//   video_source = std::make_shared<VideoSource>(channel_id,config_file);
   this->video_source = new VideoSource(channel_id,config_file);
   enum VideoSourceLogLevel level;
   level = VideoSourceLogLevel::kLOG_LEVEL_INFO;   //获取信息
   this->video_source->SetLoggingLevel(level);
   ret = this->video_source->Init();
   if (ret) {
       std::cout << "video source init failed, ret: " << ret << std::endl;
       return ret;
   }
   ret = this->video_source->Start();
   if (ret) {
       std::cout << "video source start failed, ret: " << ret << std::endl;
       return ret;
   }
   start_grab_flag = true;
   return 1;

}



void MipiCamera::startGrab()
{  
    std::shared_ptr<ImageFrame> vin_image = nullptr;
    ret = this->video_source->GetVinImageFrame(vin_image);

    if (ret) {
        std::cout << "get vin image frame failed, ret: " << ret << std::endl;
    }

    // 获取图像的宽高
    auto width = vin_image->src_info_.width;
    auto height = vin_image->src_info_.height;
    auto stride = vin_image->src_info_.stride;
    // 获取图像 y,c地址
    char* y_addr = reinterpret_cast<char*>(vin_image->src_info_.y_vaddr);
    char* c_addr = reinterpret_cast<char*>(vin_image->src_info_.c_vaddr);
    auto y_img_len = height * width;
    auto uv_img_len = height * width / 2;
    auto img_size = y_img_len + uv_img_len;
    auto *img_addr = reinterpret_cast<uint8_t *>(std::calloc(1, img_size));
    // 进行转换
    if (width == stride)
    {
        memcpy(img_addr, y_addr, y_img_len);
        memcpy(img_addr + y_img_len, c_addr, uv_img_len);
    }
    else
    {
        // copy y data jump over stride
        for (int i = 0; i < height; i++)
        {
            auto src_y_addr = y_addr + i * stride;
            auto dst_y_addr = img_addr + i * width;
            memcpy(dst_y_addr, src_y_addr, width);
        }
        // copy uv data jump over stride
        auto dst_y_size = width * height;
        for (int i = 0; i < height / 2; i++)
        {
            auto src_c_addr = c_addr + i * stride;
            auto dst_c_addr = img_addr + dst_y_size + i * width;
            memcpy(dst_c_addr, src_c_addr, width);
        }
    }
    auto image_data_size_ = width * height * 3;
    uint8_t *bgr_buf = new uint8_t[image_data_size_ / 2 + image_data_size_ * 2];
    Mat RGBImage(height,width,CV_8UC3,bgr_buf);
    Mat YUVImage(height*3/2,width,CV_8UC1, img_addr);
    cv::cvtColor(YUVImage, RGBImage, CV_YUV2BGR_NV12);
    QThread::msleep(1);
    QImage tmp;
    tmp =  matToQImage(RGBImage);
    emit sendMatImage(tmp);
    std::cout << "2222" << std::endl;
    ret = this->video_source->FreeVinImageFrame(vin_image);
    if (ret)
    {
        std::cout << "free vin image frame failed, ret: " << ret << std::endl;
    }
    RGBImage.release();
    YUVImage.release();
    free(img_addr);
    free(bgr_buf);
}

void MipiCamera::stopGrab()
{
    this->video_source->Stop();
    this->video_source->GetVinOutEnable();
    start_grab_flag = false;
}

QImage MipiCamera::matToQImage(Mat mtx)
{
    switch (mtx.type())
    {
        case CV_8UC1:
            {
                QImage img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.cols, QImage::Format_Grayscale8);
                return img;
            }
            break;
        case CV_8UC3:
            {
                QImage img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.cols * 3, QImage::Format_RGB888);
                return img.rgbSwapped();
            }
            break;
        case CV_8UC4:
            {
                QImage img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.cols * 4, QImage::Format_ARGB32);
                return img;
            }
            break;
        default:
            {
                QImage img;
                return img;
            }
    }
}




