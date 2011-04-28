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
    Settings.cpp \
    pool/Poolable.cpp \
    pool/Pool.cpp \
    pool/PoolIterator.cpp \
    waveparticles/WaveParticle.cpp \
    SkyRenderer.cpp \
    convenience/GLFileLoader.cpp \
    WaveParticleManager.cpp

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
    Settings.h \
    pool/Poolable.h \
    pool/Pool.h \
    pool/PoolIterator.h \
    waveparticles/WaveParticle.h \
    SkyRenderer.h \
    waveparticles/WaveConstants.h \
    convenience/GLFileLoader.h \
    WaveParticleManager.h

FORMS    += MainWindow.ui

INCLUDEPATH += math camera waveparticles pool convenience
DEPENDPATH += math camera waveparticles pool convenience

unix:LIBS += -L/usr/local/lib -lm -lglut
win32:LIBS += c:/mylibs/math.lib

OTHER_FILES +=

RESOURCES += \
    textures.qrc
