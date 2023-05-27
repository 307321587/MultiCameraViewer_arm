#include "drawwidget.h"
//#include <Windows.h>
#include <QDebug>
#include <QThread>

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

InteractiveView::InteractiveView(QWidget *parent,QPixmap* pixmap)
    : QGraphicsView(parent),
      m_translateButton(Qt::LeftButton),
      m_translateSpeed(1.0),
      m_zoomDelta(0.1),
      m_bMouseTranslate(false),
      m_scale(1.0),
      m_mirrored_horizontal(false),
      m_mirrored_vertical(false),
      scene(new QGraphicsScene),
      pixmap(pixmap)
{
    resize(800,600);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //关闭bar
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCursor(Qt::PointingHandCursor);      // 设置光标形状
    setRenderHint(QPainter::Antialiasing,true);  //设置渲染,启动反锯齿
    centerOn(VIEW_CENTER);

    setContextMenuPolicy(Qt::CustomContextMenu);//鼠标右键设置 设置为自定义菜单模式
    connect(this,&InteractiveView::customContextMenuRequested,this,&InteractiveView::showContextMenu);
}

void InteractiveView::setTranslateSpeed(qreal speed)
{
    m_translateSpeed = speed;
}

qreal InteractiveView::translateSpeed() const
{
    return m_translateSpeed;
}

void InteractiveView::setZoomDelta(qreal delta)
{
    m_zoomDelta = delta;
}

qreal InteractiveView::zoomDelta() const
{
    return m_zoomDelta;
}

// 上/下/左/右键向各个方向移动、加/减键进行缩放、空格/回车键旋转
void InteractiveView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        translate(QPointF(0, 2));  // 上移
        break;
    case Qt::Key_Down:
        translate(QPointF(0, -2));  // 下移
        break;
    case Qt::Key_Left:
        translate(QPointF(2, 0));  // 左移
        break;
    case Qt::Key_Right:
        translate(QPointF(-2, 0));  // 右移
        break;
    case Qt::Key_Plus:  // 放大
        zoomIn(QPoint(VIEW_CENTER));
        break;
    case Qt::Key_Minus:  // 缩小
        zoomOut(QPoint(VIEW_CENTER));
        break;
    case Qt::Key_Space:  // 逆时针旋转
        rotate(-5);
        break;
    case Qt::Key_Enter:  // 顺时针旋转
        rotate(5);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

// 平移
void InteractiveView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMouseTranslate){
        QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
//        QPointF mouseDelta = mapToScene(this->pos()) - mapToScene(m_lastMousePos);
        translate(mouseDelta);
    }
    m_lastMousePos = event->pos();
//    m_lastMousePos = this->pos();

    // 获取鼠标RGB值
    int x = QCursor::pos().x();
    int y = QCursor::pos().y();
    QList<QScreen *> list_screen =  QGuiApplication::screens();
    QPixmap pixmap = list_screen.at(0)->grabWindow(0,x,y,1,1);
    if (!pixmap.isNull()) //如果像素图不为NULL
        {
            QImage image = pixmap.toImage();//将像素图转换为QImage
            if (!image.isNull()) //如果image不为空
            {
                if (image.valid(0, 0)) //坐标位置有效
                {
                    QColor color = image.pixel(0, 0);
                    int mousedPressed_R = color.red();
                    int mousedPressed_G = color.green();
                    int mousedPressed_B = color.blue();
                    emit send_RGBVal(mousedPressed_R,mousedPressed_G,mousedPressed_B);
//                    QString text = QString("RGB: %1, %2, %3").arg(mousedPressed_R).arg(mousedPressed_G).arg(mousedPressed_B);
//                    qDebug() << text << list_screen.size();
                }
            }
        }




    QGraphicsView::mouseMoveEvent(event);
}

void InteractiveView::mousePressEvent(QMouseEvent *event) // 鼠标拖动移动
{
    if (event->button() == m_translateButton) {
        QCursor cursor;                         //创建光标对象
        cursor.setShape(Qt::OpenHandCursor);    //设置光标形态为手掌
        setCursor(cursor);
        m_bMouseTranslate = true;
        m_lastMousePos = event->pos();
//        m_lastMousePos = this->pos();
    }
    QGraphicsView::mousePressEvent(event);
}

void InteractiveView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == m_translateButton)
    {
        QCursor cursor;                          //创建光标对象
        cursor.setShape(Qt::ArrowCursor);        //设置光标形态为箭头
        setCursor(cursor);                       //使用手掌光标
        m_bMouseTranslate = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// 上下移动 / +ctrl 放大缩小 /+ shfit 左旋右旋
void InteractiveView::wheelEvent(QWheelEvent *event)
{
    QPoint scrollAmount = event->angleDelta(); //记录鼠标滚轮滚动角度
    QPoint MousePos = event->pos();
    if(QApplication::keyboardModifiers()==Qt::ControlModifier)      //+ctrl 放大缩小
    {
        scrollAmount.y() > 0 ? zoomIn(MousePos) : zoomOut(MousePos);
    }if(QApplication::keyboardModifiers()==Qt::ShiftModifier)   //+ shfit 左旋右旋
    {
        centerOn(mapToScene(VIEW_CENTER));
        scrollAmount.y() > 0 ? rotate(2) : rotate(-2);
    }
    else{                                                        //上下移动
        scrollAmount.y() > 0 ? translate(QPointF(0, 4)) :translate(QPointF(0, -4));
    }
}

void InteractiveView::zoomIn(QPoint focus)
{
    zoom(1 + m_zoomDelta,focus);
}


void InteractiveView::zoomOut(QPoint focus)
{
    zoom(1 - m_zoomDelta,focus);
}

void InteractiveView::zoom(float scaleFactor,QPoint focus)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
    centerOn(mapToScene(focus));    //设置zoom中心
    scale(scaleFactor, scaleFactor);
    m_scale *= scaleFactor;
}

void InteractiveView::translate(QPointF delta)
{
    // 根据当前 zoom 缩放平移数
    delta *= m_scale;
    delta *= m_translateSpeed;
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(),  VIEW_HEIGHT / 2 - delta.y());
//    QPoint newCenter(this->width() / 2 - delta.x(),  this->height() / 2 - delta.y());
    centerOn(mapToScene(newCenter));
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

}

void InteractiveView::showContextMenu()
{
    QMenu *pMenu = new QMenu(this);
    QAction* pMenuFit = new QAction("Fit In View",this);
    QAction* pMenuZoom = new QAction("Zoom",this);
    QAction* pMenuZoomIn = new QAction("Zoom In",this);
    QAction* pMenuZoomOut = new QAction("Zoom Out",this);
    QAction* pMenuLrotation = new QAction("Anticlockwise Rotation",this);
    QAction* pMenuRrotation = new QAction("Clockwise Rotation",this);
    QAction* pMenuloadImage = new QAction("Loading Image",this);
    QAction* pMenusave = new QAction("Save Image",this);
    QAction* pMenuclean = new QAction("Clean",this);

    pMenu->addAction(pMenuFit);
    pMenu->addAction(pMenuZoom);
    pMenu->addAction(pMenuZoomIn);
    pMenu->addAction(pMenuZoomOut);
    pMenu->addSeparator();
    pMenu->addAction(pMenuLrotation);
    pMenu->addAction(pMenuRrotation);
    pMenu->addSeparator();
    pMenu->addAction(pMenuloadImage);
    pMenu->addAction(pMenusave);
    pMenu->addAction(pMenuclean);

    connect(pMenuFit,&QAction::triggered,this,&InteractiveView::menu_fit);
    connect(pMenuZoom,&QAction::triggered,this,&InteractiveView::menu_zoom);
    connect(pMenuZoomIn,&QAction::triggered,this,&InteractiveView::menu_zoomIn);
    connect(pMenuZoomOut,&QAction::triggered,this,&InteractiveView::menu_zoomOut);
    connect(pMenuLrotation,&QAction::triggered,this,&InteractiveView::menu_Lrotation);
    connect(pMenuRrotation,&QAction::triggered,this,&InteractiveView::menu_Rrotation);
    connect(pMenuloadImage,&QAction::triggered,this,&InteractiveView::menu_loadImage);
    connect(pMenusave,&QAction::triggered,this,&InteractiveView::menu_save);
    connect(pMenuclean,&QAction::triggered,this,&InteractiveView::menu_clean);

    pMenu->exec(cursor().pos()); //设置在右键点击的地方出现
    //释放内存
    QList<QAction*> list = pMenu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete pMenu;
}

void InteractiveView::menu_fit()
{
    fitInView(scene->itemsBoundingRect());
    QRectF rectItem = scene->itemsBoundingRect();
    QRectF rectView = this->rect();
    qreal ratioView = rectView.height() / rectView.width();
    qreal ratioItem = rectItem.height() / rectItem.width();
    if (ratioView > ratioItem)
    {
        rectItem.moveTop(rectItem.width()*ratioView - rectItem.height());
        rectItem.setHeight(rectItem.width()*ratioView);

        rectItem.setWidth(rectItem.width() * 1.05);
        rectItem.setHeight(rectItem.height() * 1.05);
    }
    else
    {
        rectItem.moveLeft(rectItem.height()/ratioView - rectItem.width());
        rectItem.setWidth(rectItem.height()/ratioView);

         rectItem.setWidth(rectItem.width() * 1.05);
        rectItem.setHeight(rectItem.height() * 1.05);
    }

    this->fitInView(rectItem, Qt::KeepAspectRatio);
}

void InteractiveView::menu_zoom()
{
    this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    QMatrix q;
    q.setMatrix(1,this->matrix().m12(),this->matrix().m21(),1,this->matrix().dx(),this->matrix().dy());
//    centerOn(mapToScene(focus));
    this->setMatrix(q,false);
    this->centerOn(this->mapToScene(QPoint(0,0)));
}

void InteractiveView::menu_zoomIn()
{
    zoomIn(cursor().pos());
}

void InteractiveView::menu_zoomOut()
{
    zoomOut(cursor().pos());
}

void InteractiveView::menu_Lrotation()
{
    rotate(-90);
}

void InteractiveView::menu_Rrotation()
{
    rotate(90);
}

void InteractiveView::menu_loadImage()
{
    if(pixmap == nullptr){
        pixmap = new QPixmap;
        QString filePath;
        filePath = QFileDialog::getOpenFileName(this,"Loading Image","./../../","Imagers (*.jpg *png);;All Files (*)",0);
        if(filePath==NULL){
            QMessageBox::information(NULL,"Error!","No Photo!");
            pixmap = nullptr;
        }
        else{
            centerOn(VIEW_CENTER);
            pixmap->load(filePath);
            scene->addPixmap(*pixmap);
            this->setScene(scene);
        }
    }else
    {
        centerOn(VIEW_CENTER);
        scene->addPixmap(*pixmap);
        this->setScene(scene);
    }
}

void InteractiveView::menu_save()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //防止截图把scrollbar截进去
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QPixmap savePixmap =  this->grab();
    QString saveFilePath = QFileDialog::getSaveFileName(this, tr("Save Image"),"./../../",tr("Images (*.png *.jpg)"));
    savePixmap.save(saveFilePath);
}

void InteractiveView::menu_clean()
{
    if(scene != NULL){
        scene->clear();
        pixmap = nullptr;
        scene->destroyed(this);
        centerOn(VIEW_CENTER);
        this->menu_fit();
    }
}

void InteractiveView::show_Qimage(QImage img)
{
    if(scene != NULL)
    {
        scene->clear();
        this->scene->addPixmap(QPixmap::fromImage(img.mirrored(m_mirrored_horizontal,m_mirrored_vertical)));
        this->setScene(scene);
    }
    else
    {
        this->scene->addPixmap(QPixmap::fromImage(img.mirrored(m_mirrored_horizontal,m_mirrored_vertical)));
        this->setScene(scene);
    }
}

