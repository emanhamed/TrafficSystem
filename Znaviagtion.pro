#-------------------------------------------------
#
# Project created by QtCreator 2016-03-31T20:24:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Znaviagtion
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann


SOURCES += \
    Color.cpp \
    CU.cpp

HEADERS += \
    Color.h
