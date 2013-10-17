#include "ObjectViewer.h"

const qglviewer::Vec ObjectViewer::defaultColor(0.5f, 0.5f, 0.5f);
const float ObjectViewer::distanceBetweenSampleForRendering(0.01f);

ObjectViewer::ObjectViewer(Mesh* const mesh)
    :mesh(mesh), vMode(point), color(defaultColor)
{

}

void ObjectViewer::init()
{
    restoreStateFromFile();
    startAnimation();

    setGridIsDrawn();
    setAxisIsDrawn();
    setFPSIsDisplayed(true);

    /*glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW); // front face -> clock wise enumeration*/

    this->camera()->setPosition(qglviewer::Vec(0.0f, -2.5f, 2.0f));
    this->camera()->setUpVector(qglviewer::Vec(0.0f, 0.0f, 1.0f));
    this->camera()->lookAt(qglviewer::Vec(0.0f, 0.0f, 0.0f));
    this->setBackgroundColor(QColor(255,255,255));
    resize(800, 600);
}

 void ObjectViewer::draw()
{

     switch(vMode) {
     case wireframe:
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         break;
     case flat:
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
         glShadeModel(GL_FLAT);
         break;
     case smooth:
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
         glShadeModel(GL_SMOOTH);
         break;
     }

     // ou cas ou, faut que je regarde sa: http://www.libqglviewer.com/examples/manipulatedFrame.html
     // si on doit utiliser matrix, assez etrange avec opengl
     //glPushMatrix();
     //glScalef(0.5f, 0.5f, 0.5f);
     //glScalef(0.5f, 0.5f, 0.5f);

     switch(vMode) {
        case wireframe:
        case flat:
        case smooth:
            color = defaultColor;
            glEnable(GL_LIGHTING);
            drawTriangles();
            break;
         case point:
             color = qglviewer::Vec(0.0f, 0.0f, 0.0f);
             glDisable(GL_LIGHTING);
             drawPoints(false);
             break;
         case pointSampled:
             color = qglviewer::Vec(0.0f, 0.0f, 0.0f);
             glDisable(GL_LIGHTING);
             drawPoints(true);
             break;
     }

     //glPopMatrix();
     // restore, to correctly show the axis and the fps
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glShadeModel(GL_SMOOTH);
}

 void ObjectViewer::drawTriangles()
 {
     glBegin(GL_TRIANGLES);

     const std::vector<qglviewer::Vec *> triangles = mesh->getTriangles();
     unsigned int size = triangles.size();
     for( unsigned int i(0); i < size; ++i)
     {
         glColor3fv(color);
         glVertex3fv(*triangles[i]);
     }

     glEnd();
 }

 void ObjectViewer::drawPoints(bool sampledPoint)
 {
     glPointSize(1.0f);
     glBegin(GL_POINTS);

     std::vector<qglviewer::Vec *> points;
     if (sampledPoint){
        points = mesh->getPoints(true, distanceBetweenSampleForRendering);
     } else {
        points = mesh->getPoints();
     }

     unsigned int size = points.size();
     for( unsigned int i(0); i < size; ++i)
     {
         glColor3fv(color);
         glVertex3fv(*points[i]);
     }

     glEnd();
 }

 // press enter to stop/start animation
 void ObjectViewer::animate()
 {
     mesh->update();
 }

 void ObjectViewer::closeEvent(QCloseEvent *event)
 {
     emit closeSignal();
     event->accept();
 }

 void ObjectViewer::closeViewer()
 {
     this->close();
 }

 void ObjectViewer::wireframeMode()
 {
     vMode = wireframe;

     // tell the mesh to generate new point/triangle
     mesh->setUpdateOnMesh();
 }

 void ObjectViewer::flatMode()
 {
    vMode = flat;

    // tell the mesh to generate new point/triangle
    mesh->setUpdateOnMesh();
 }


 void ObjectViewer::smoothMode()
 {
     vMode = smooth;

     // tell the mesh to generate new point/triangle
     mesh->setUpdateOnMesh();
 }

 void ObjectViewer::pointMode()
 {
     vMode = point;

     // tell the mesh to generate new point/triangle
     mesh->setUpdateOnMesh();
 }

 void ObjectViewer::pointSampledMode()
 {
     vMode = pointSampled;

     // tell the mesh to generate new point/triangle
     mesh->setUpdateOnMesh();
 }

