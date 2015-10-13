#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T13:36:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImgP
TEMPLATE = app

RC_FILE = imageview.rc

RESOURCES += \
    images.qrc

FORMS += \
    imageview.ui \
    GLSM.ui \
    dialogcamera.ui


HEADERS += \
    imageview.h \
    Convert.h \
    glcm.h \
    levelsetseg.h \
    dialogcamera.h \
    capture.h

SOURCES += \
    imageview.cpp \
    main.cpp \
    glcm.cpp \
    levelsetseg.cpp \
    Convert.cpp \
    dialogcamera.cpp \
    capture.cpp


INCLUDEPATH += D:\Program\opencv\build\include\opencv\
               D:\Program\opencv\build\include\opencv2\
               D:\Program\opencv\build\include

LIBS += -LD:\Program\opencv\build\x86\vc10\lib\
        -lopencv_core247 \
        -lopencv_highgui247 \
        -lopencv_imgproc247 \
        -lopencv_features2d247 \
        -lopencv_calib3d247
