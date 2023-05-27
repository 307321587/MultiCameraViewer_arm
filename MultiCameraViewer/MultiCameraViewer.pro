#-------------------------------------------------
#
# Project created by QtCreator 2022-01-12T14:21:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MultiCameraViewer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    ffmpegdecode.cpp \
        main.cpp \
        mainwindow.cpp \
    drawwidget.cpp \
    ui_camerawidget.cpp

HEADERS += \
    ffmpegdecode.h \
        mainwindow.h \
    drawwidget.h \
    ui_camerawidget.h

FORMS += \
    mainwindow.ui \
    ui_camera_viewer.ui

unix:!macx: LIBS += -L$$PWD/../ffmpeg-arm/lib/ -lavutil -lavformat -lavcodec -lavdevice -lavfilter -lpostproc -lswresample -lswscale

INCLUDEPATH += $$PWD/../ffmpeg-arm/include
DEPENDPATH += $$PWD/../ffmpeg-arm/include

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

mylibs.path = /opt/$${TARGET}/lib/
mylibs.files+=$$PWD/../ffmpeg-arm/lib/
INSTALLS+=mylibs
LIBS += -L/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/lib -ldl





