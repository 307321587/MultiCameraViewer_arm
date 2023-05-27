#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QObject>
#include <QScreen>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QDialog>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QMenu>
#include <QAction>
#include <QApplication>
//#include <opencv2/opencv.hpp>
#include <QObject>

class QWheelEvent;
class QKeyEvent;

class InteractiveView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit InteractiveView(QWidget *parent = 0,QPixmap* pixmap = nullptr);

    // 平移速度
    void setTranslateSpeed(qreal speed);
    qreal translateSpeed() const;

    // 缩放的增量
    void setZoomDelta(qreal delta);
    qreal zoomDelta() const;

protected:
    // 上/下/左/右键向各个方向移动、加/减键进行缩放、空格/回车键旋转
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void zoomIn(QPoint focus);
    void zoomOut(QPoint focus);
    void zoom(float scaleFactor,QPoint focus);
    void translate(QPointF delta);
    void showContextMenu(); //设置鼠标右键显示

    void menu_fit();
    void menu_zoom();       //和上方的zoom不同，不考虑鼠标焦点
    void menu_zoomIn();
    void menu_zoomOut();
    void menu_Lrotation();
    void menu_Rrotation();
    void menu_loadImage();
    void menu_save();
    void menu_clean();

    void set_mirrored(bool H, bool V){m_mirrored_horizontal = H;m_mirrored_vertical =V;}
    bool is_horizontal(){return m_mirrored_horizontal;}
    bool is_vertical(){return m_mirrored_vertical;}
signals:
    void send_RGBVal(int R, int G, int B);

private:
    Qt::MouseButton m_translateButton;  // 平移按钮
    qreal m_translateSpeed;  // 平移速度
    qreal m_zoomDelta;  // 缩放的增量
    bool m_bMouseTranslate;  // 平移标识
    QPoint m_lastMousePos;  // 鼠标最后按下的位置
    qreal m_scale;  // 缩放值
    bool m_mirrored_horizontal;    //水平镜像翻转
    bool m_mirrored_vertical;    //垂直镜像翻转

public:
    QGraphicsScene *scene;
    QPixmap* pixmap;         //用来存放 图片

    void show_Qimage(QImage img);
};

#endif // DRAWWIDGET_H
