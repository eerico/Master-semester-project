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
    GarbageCollector/ProfileDestructorManager.cpp \
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
    #Reconstruction3D/Plan.cpp \
    #Reconstruction3D/ActivePlan.cpp \
    #Reconstruction3D/Chains.cpp \
    GUI/AllChainScene.cpp \
    GarbageCollector/GeneralDestructorManager.cpp \
    GUI/SimplificationWindow.cpp \
    Simplification/Simplification.cpp \
    GUI/SimplificationFloorPlanScene.cpp \
    GUI/SimplificationProfileScene.cpp \
    GUI/SimplificationScene.cpp \
    GUI/ProfileMergeWindow.cpp \
    GUI/ProfileMergeScene.cpp \
    GUI/PreviewScene.cpp \
    Reconstruction3D/ActivePlan.cpp \
    Reconstruction3D/Plan.cpp \
    Reconstruction3D/GeneralEvent.cpp \
    Reconstruction3D/Event.cpp \
    Reconstruction3D/EdgeEvent.cpp
    GUI/PreviewScene.cpp \
    #Reconstruction3D_old/Reconstruction3D.cpp \
    #Reconstruction3D_old/Plan.cpp \
    #Reconstruction3D_old/Chain.cpp \
    #Reconstruction3D_old_Tom_Kelly/Reconstruction3D.cpp \
    #Reconstruction3D_old_Tom_Kelly/Plan.cpp \
    #Reconstruction3D_old_Tom_Kelly/Chain.cpp

HEADERS  += \
    FloorPlanAndProfileExtractor.h \
    GarbageCollector/ProfileDestructorManager.h \
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
    #Reconstruction3D/Plan.h \
    #Reconstruction3D/IntersectionComparator.h \
    #Reconstruction3D/Intersection.h \
    #Reconstruction3D/ActivePlan.h \
    #Reconstruction3D/Chains.h \
    #Reconstruction3D/ChainOrientationComparator.h \
    GUI/AllChainScene.h \
    GarbageCollector/GarbageCollectorObject.h \
    GarbageCollector/GeneralDestructorManager.h \
    GUI/SimplificationWindow.h \
    #Reconstruction3D_old/Reconstruction3D.h \
    #Reconstruction3D_old/Plan.h \
    #Reconstruction3D_old/Intersection.h \
    #Reconstruction3D_old/IntersectionComparator.h \
    #Reconstruction3D_old/Chain.h \
    #Reconstruction3D_old_Tom_Kelly/Reconstruction3D.h \
    #Reconstruction3D_old_Tom_Kelly/Plan.h \
    #Reconstruction3D_old_Tom_Kelly/IntersectionComparator.h \
    #Reconstruction3D_old_Tom_Kelly/Intersection.h \
    #Reconstruction3D_old_Tom_Kelly/Chain.h
    Simplification/Curve.h \
    Simplification/Simplification.h \
    GUI/SimplificationFloorPlanScene.h \
    GUI/SimplificationProfileScene.h \
    GUI/SimplificationScene.h \
    GUI/ProfileMergeWindow.h \
    GUI/ProfileMergeScene.h \
    GUI/PreviewScene.h \
    Reconstruction3D/ActivePlan.h \
    Reconstruction3D/Plan.h \
    Reconstruction3D/GeneralEvent.h \
    Reconstruction3D/Event.h \
    Reconstruction3D/EdgeEvent.h
    GUI/PreviewScene.h \

#QT *= xml opengl

#CONFIG += qt opengl warn_on thread rtti console embed_manifest_exe


win32{
    #path to where is the libqglviewer root
    LIBQGL_PATH = "C:/Program Files (x86)/libQGLViewer"

    win32:INCLUDEPATH += $$LIBQGL_PATH/sources
    win32:DEPENDPATH += $$LIBQGL_PATH/sources


    # LIB_NAME

    LIB_NAME = QGLViewer
    build_pass:CONFIG(debug, debug|release) {
        LIB_NAME = $$join(LIB_NAME,,,d)
    }
    LIB_NAME = $$join(LIB_NAME,,,2)

    win32:LIBS += -L$$LIBQGL_PATH/lib/QGLViewer -l$$LIB_NAME


    win32:LIBS += -L"C:\Program Files (x86)\OpenMesh\lib" -lOpenMeshCore

    INCLUDEPATH += "C:\Program Files (x86)\OpenMesh\include"
    DEPENDPATH += "C:\Program Files (x86)\OpenMesh\include"

    win32:LIBS += -L"C:\Program Files (x86)\OpenMesh\lib" -lOpenMeshTools

    DEFINES +=  _USE_MATH_DEFINES
}


unix:macx {
    INCLUDEPATH *= /Users/erico/Qt/libQGLViewer-2.4.0
    INCLUDEPATH *= /Users/erico/Downloads/OpenMesh-2.4/src/
    LIBS *= -F/Users/erico/Library/Frameworks -framework QGLViewer
    LIBS += -L/Users/erico/Downloads/OpenMesh-2.4/om/Build/lib/OpenMesh -lOpenMeshCored -lOpenMeshToolsd
}
