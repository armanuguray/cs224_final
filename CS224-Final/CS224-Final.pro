#-------------------------------------------------
#
# Project created by QtCreator 2011-04-08T13:35:02
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG += qt warn_on precompile_header

TARGET = CS224-Final
TEMPLATE = app

PRECOMPILED_HEADER = CS224-Final_Prefix.pch

SOURCES += main.cpp \
        MainWindow.cpp \
    GLWidget.cpp \
    math/CS123Vector.inl \
    math/CS123Matrix.inl \
    math/CS123Matrix.cpp \
    DrawEngine.cpp \
    camera/Camera.cpp \
    camera/ProjectorCamera.cpp \
    Settings.cpp

HEADERS  += MainWindow.h \
    CS224-Final_Prefix.pch \
    GLWidget.h \
    math/CS123Vector.h \
    math/CS123Matrix.h \
    math/CS123Algebra.h \
    DrawEngine.h \
    camera/Camera.h \
    OpenGLInclude.h \
    camera/ProjectorCamera.h \
    Settings.h

FORMS    += MainWindow.ui

INCLUDEPATH += math camera
DEPENDPATH += math camera

unix:LIBS += -L/usr/local/lib -lm
win32:LIBS += c:/mylibs/math.lib
