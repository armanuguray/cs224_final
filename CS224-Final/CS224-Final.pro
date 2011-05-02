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
    WaveParticleManager.cpp \
    rigidbody/RigidBody.cpp

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
    WaveParticleManager.h \
    rigidbody/RigidBody.h

FORMS    += MainWindow.ui

QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg

INCLUDEPATH += math camera waveparticles pool convenience ../bullet/include/bullet
DEPENDPATH += math camera waveparticles pool convenience

unix:!mac {
    LIBS += -L$$_PRO_FILE_PWD_/../bullet/lib
}

mac {
    LIBS += -L$$_PRO_FILE_PWD_/../bullet-mac/lib
}

unix:LIBS += -L/usr/local/lib -lm \
-lBulletMultiThreaded \
-lBulletSoftBody \
-lBulletDynamics \
-lBulletCollision \
-lLinearMath
win32:LIBS += c:/mylibs/math.lib

OTHER_FILES +=

RESOURCES += \
    textures.qrc
