#include "ObjectViewer.h"

const qglviewer::Vec ObjectViewer::defaultColor(0.5f, 0.5f, 0.5f);
const float ObjectViewer::distanceBetweenSampleForRendering(0.01f);

ObjectViewer::ObjectViewer(MeshManager* const meshManager)
    :meshManager(meshManager), vMode(noView), color(defaultColor)
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
     case noView:
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
         drawPoints();
         break;
     case noView:
         meshManager->clearTriangles();
         meshManager->clearPoints();
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

     const std::vector<qglviewer::Vec *>* triangles = meshManager->getTriangles();
     unsigned int size = triangles->size();
     /*for( unsigned int i(0); i < size; ++i)
     {
         glColor3fv(color);
         glVertex3fv(*triangles[i]);
     }*/

     float normalX(0.0f);
     float normalY(0.0f);
     float normalZ(0.0f);

     for( unsigned int i(0); i < size; i = i + 3)
     {
         qglviewer::Vec vertex1 = *(*triangles)[i];
         qglviewer::Vec vertex2 = *(*triangles)[i+1];
         qglviewer::Vec vertex3 = *(*triangles)[i+2];
         qglviewer::Vec vector1 = vertex3 - vertex1;
         qglviewer::Vec vector2 = vertex2 - vertex1;

         Utils::crossProduct(vector1[0], vector1[1], vector1[2],
                            vector2[0], vector2[1], vector2[2],
                            normalX, normalY, normalZ);

         qglviewer::Vec normal(normalX, normalY, normalZ);
         normal.normalize();

         glNormal3fv(normal);
         glColor3fv(color);
         glVertex3fv(vertex1);

         glNormal3fv(normal);
         glColor3fv(color);
         glVertex3fv(vertex2);

         glNormal3fv(normal);
         glColor3fv(color);
         glVertex3fv(vertex3);
     }

     glEnd();
}

void ObjectViewer::drawPoints()
{
     glPointSize(2.0f);
     glBegin(GL_POINTS);

     std::vector<qglviewer::Vec *>* points;
     points = meshManager->getPoints();

     unsigned int size = points->size();
     for( unsigned int i(0); i < size; ++i)
     {
         glColor3fv(color);
         glVertex3fv(*(*points)[i]);
     }

     glEnd();
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

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}

void ObjectViewer::flatMode()
{
    vMode = flat;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}


void ObjectViewer::smoothMode()
{
    vMode = smooth;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}

void ObjectViewer::pointMode()
{
    vMode = point;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}

void ObjectViewer::noViewMode()
{
    vMode = noView;
}
