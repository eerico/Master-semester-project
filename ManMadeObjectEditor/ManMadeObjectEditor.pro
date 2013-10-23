#-------------------------------------------------
#
# Project created by QtCreator 2013-10-01T23:54:17
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ManMadeObjectEditor
TEMPLATE = app


SOURCES +=\
        ObjectViewer.cpp \
    Main.cpp \
    Mesh.cpp \
    Profile.cpp \
    FloorAndProfileViewer.cpp \
    CentralWidget.cpp \
    FloorScene.cpp \
    ProfileScene.cpp \
    Vertex.cpp \
    FloorPlanAndProfileExtractor.cpp \
    BasicQGraphicsView.cpp \
    ProfileDestructorManager.cpp \
    Utils.cpp \
    Edge.cpp

HEADERS  += \
    Mesh.h \
    ObjectViewer.h \
    Profile.h \
    FloorAndProfileViewer.h \
    CentralWidget.h \
    FloorScene.h \
    ProfileScene.h \
    Vertex.h \
    FloorPlanAndProfileExtractor.h \
    BasicQGraphicsView.h \
    ProfileDestructorManager.h \
    Utils.h \
    Edge.h

#QT *= xml opengl

#CONFIG += qt opengl warn_on thread rtti console embed_manifest_exe


win32{
    INCLUDEPATH *= D:\Qt\libQGLViewer-2.4.0
    LIBS *= -LD:\Qt\libQGLViewer-2.4.0/QGLViewer -lQGLViewer2

    win32:CONFIG(release, debug|release): LIBS += -LD:\OpenMesh\lib\ -lOpenMeshCore
    else:win32:CONFIG(debug, debug|release): LIBS += -LD:\OpenMesh\lib\ -lOpenMeshCored

    INCLUDEPATH += D:\OpenMesh\include
    DEPENDPATH += D:\OpenMesh\include

    win32:CONFIG(release, debug|release): LIBS += -LD:\OpenMesh\lib\ -lOpenMeshTools
    else:win32:CONFIG(debug, debug|release): LIBS += -LD:\OpenMesh\lib\ -lOpenMeshToolsd

    DEFINES +=  _USE_MATH_DEFINES
}


unix:macx {
    INCLUDEPATH *= /Users/erico/Qt/libQGLViewer-2.4.0
    INCLUDEPATH *= /Users/erico/Downloads/OpenMesh-2.4/src/
    LIBS *= -F/Users/erico/Library/Frameworks -framework QGLViewer
    LIBS += -L/Users/erico/Downloads/OpenMesh-2.4/om/Build/lib/OpenMesh -lOpenMeshCored -lOpenMeshToolsd
}
