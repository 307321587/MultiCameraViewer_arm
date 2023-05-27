#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_camerawidget.h"
#include <QTimer>
#include <QDateTime>
#include <iostream>
#include <vector>
#include <QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->retranslateUi(this);

    connect(ui->btn_create_window,SIGNAL(clicked(bool)),this,SLOT(btn_window_create()));
    connect(ui->btn_delete_window,SIGNAL(clicked(bool)),this,SLOT(btn_window_delete()));
    connect(ui->btn_all_start,SIGNAL(clicked(bool)),this,SLOT(btn_all_start()));
    connect(ui->btn_all_stop,SIGNAL(clicked(bool)),this,SLOT(btn_all_stop()));
    connect(ui->btn_all_image_save,SIGNAL(clicked(bool)),this,SLOT(btn_all_image_save()));
    connect(ui->btn_refresh,SIGNAL(clicked(bool)),this,SLOT(btn_refresh()));
    ui->btn_all_image_save->setEnabled(false);
    ui->btn_all_start->setEnabled(false);
    ui->btn_all_stop->setEnabled(false);
    ui->btn_all_image_save->setEnabled(false);

    QTimer *timer;
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        QWidget::update();
    });
    timer->start(10);
}

void MainWindow::btn_window_create()
{
    if(MainWindow::windows.size()< 4 ){
        int last_window_num = MainWindow::windows.size();
        CameraWidget *w1 = new CameraWidget;
        MainWindow::windows.push_back(w1);
        ui->gridLayout->addWidget(windows[last_window_num],window_position_row[last_window_num],window_position_col[last_window_num]);

        ui->btn_all_image_save->setEnabled(true);
        ui->btn_all_start->setEnabled(true);
        ui->btn_all_stop->setEnabled(true);
        ui->btn_all_image_save->setEnabled(true);
    }
    else{
        QMessageBox::information(this,"Error","Currently supports up to 4 windows!");
    }
}

void MainWindow::btn_window_delete()
{
    int last_window_num = MainWindow::windows.size()-1;
    if(MainWindow::windows.size()>0)
    {
        ui->gridLayout->removeWidget(MainWindow::windows[last_window_num]);
        CameraWidget* w1 = windows[last_window_num];
        w1->btn_camera_close();
        w1->setParent(nullptr);
        MainWindow::windows.pop_back();
        if(MainWindow::windows.size()==0)
        {
            ui->btn_all_image_save->setEnabled(false);
            ui->btn_all_start->setEnabled(false);
            ui->btn_all_stop->setEnabled(false);
            ui->btn_all_image_save->setEnabled(false);
        }
    }
}

void MainWindow::btn_all_start()
{
    if(windows.size()==0)
    {
        QMessageBox::information(this,"Error","No open windows!");
    }
    else
    {
        for(int i=0;i < (int) windows.size();i++)
        {
            windows[i]->btn_camera_open();
            windows[i]->btn_start_grab();
        }
    }
}

void MainWindow::btn_all_stop()
{
    if(windows.size()==0)
    {
        QMessageBox::information(this,"Error","No open windows!");
    }
    else
    {
        for(int i=0;i < (int) windows.size();i++)
        {
            windows[i]->btn_camera_close();
        }
    }
}

void MainWindow::btn_refresh()
{
    for(int i=0;i < (int) windows.size();i++)
    {
        windows[i]->camera_update();
    }
}

void MainWindow::btn_all_image_save()
{
    if(windows.size()==0)
    {
        QMessageBox::information(this,"Error","No open windows!");
    }
    else
    {
        for(int i=0;i < (int) windows.size();i++)
        {
            windows[i]->btn_image_shoot();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
