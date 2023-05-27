#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_camerawidget.h"

#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



public slots:
    void btn_window_create();
    void btn_window_delete();
    void btn_all_start();
    void btn_all_stop();
    void btn_refresh();
    void btn_all_image_save();

private:
    Ui::MainWindow *ui;
    int window_position_row[4] = {0,0,1,1};
    int window_position_col[4] = {0,1,0,1};
    std::vector<CameraWidget*> windows;

};

#endif // MAINWINDOW_H
