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
    Main.cpp \
    FloorPlanAndProfileExtractor.cpp \
    ProfileDestructorManager.cpp \
    Utils.cpp \
    MeshManager.cpp \
    Test.cpp \
    GUI/ObjectViewer.cpp \
    GUI/FloorScene.cpp \
    GUI/FloorAndProfileViewer.cpp \
    GUI/CentralWidget.cpp \
    GUI/BasicQGraphicsView.cpp \
    GUI/AllPlanScene.cpp \
    Geometry/Vertex.cpp \
    Geometry/Profile.cpp \
    Geometry/Edge.cpp \
    GUI/ProfileScene.cpp \
    Reconstruction3D/Reconstruction3D.cpp \
    Reconstruction3D/Plan.cpp \
    Reconstruction3D/Chain.cpp \
    Reconstruction3D_old/Reconstruction3D.cpp \
    Reconstruction3D_old/Plan.cpp \
    Reconstruction3D_old/Chain.cpp

HEADERS  += \
    FloorPlanAndProfileExtractor.h \
    ProfileDestructorManager.h \
    Utils.h \
    MeshManager.h \
    Test.h \
    GUI/ObjectViewer.h \
    GUI/FloorScene.h \
    GUI/FloorAndProfileViewer.h \
    GUI/CentralWidget.h \
    GUI/BasicQGraphicsView.h \
    GUI/AllPlanScene.h \
    Geometry/Vertex.h \
    Geometry/Profile.h \
    Geometry/Edge.h \
    GUI/ProfileScene.h \
    Reconstruction3D/Reconstruction3D.h \
    Reconstruction3D/Plan.h \
    Reconstruction3D/Intersection.h \
    Reconstruction3D/IntersectionComparator.h \
    Reconstruction3D/Chain.h \
    Reconstruction3D_old/Reconstruction3D.h \
    Reconstruction3D_old/Plan.h \
    Reconstruction3D_old/IntersectionComparator.h \
    Reconstruction3D_old/Intersection.h \
    Reconstruction3D_old/Chain.h

#QT *= xml opengl

#CONFIG += qt opengl warn_on thread rtti console embed_manifest_exe


win32{
    INCLUDEPATH *= "C:\Program Files (x86)\libQGLViewer-2.4.0"
    LIBS *= -L"C:\Program Files (x86)\libQGLViewer-2.4.0/QGLViewer" -lQGLViewer2

    win32:CONFIG(release, debug|release): LIBS += -L"C:\Program Files (x86)\OpenMesh\lib" -lOpenMeshCore
    else:win32:CONFIG(debug, debug|release): LIBS += -L"C:\Program Files (x86)\OpenMesh\lib" -lOpenMeshCored

    INCLUDEPATH += "C:\Program Files (x86)\OpenMesh\include"
    DEPENDPATH += "C:\Program Files (x86)\OpenMesh\include"

    win32:CONFIG(release, debug|release): LIBS += -L"C:\Program Files (x86)\OpenMesh\lib" -lOpenMeshTools
    else:win32:CONFIG(debug, debug|release): LIBS += -L"C:\Program Files (x86)\OpenMesh\lib" -lOpenMeshToolsd

    DEFINES +=  _USE_MATH_DEFINES
}


unix:macx {
    INCLUDEPATH *= /Users/erico/Qt/libQGLViewer-2.4.0
    INCLUDEPATH *= /Users/erico/Downloads/OpenMesh-2.4/src/
    LIBS *= -F/Users/erico/Library/Frameworks -framework QGLViewer
    LIBS += -L/Users/erico/Downloads/OpenMesh-2.4/om/Build/lib/OpenMesh -lOpenMeshCored -lOpenMeshToolsd
}
