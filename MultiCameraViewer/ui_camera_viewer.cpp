#include "ui_camera_viewer.h"
#include "ui_mainwindow.h"

//MainWindow::MainWindow(QWidget *parent) :
//    QMainWindow(parent),
//    ui(new Ui::MainWindow)
//{
//    ui->setupUi(this);
//}


ui_camera_viewer::ui_camera_viewer(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::ui_camera_viewer)
{
    ui->setupUi(this);
}


//MainWindow::~MainWindow()
//{
//    delete ui;
//}
ui_camera_viewer::~ui_camera_viewer()
{
    delete ui;
}
