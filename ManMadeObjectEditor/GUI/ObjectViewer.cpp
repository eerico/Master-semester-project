#include "ObjectViewer.h"

const qglviewer::Vec ObjectViewer::defaultColor(0.5f, 0.5f, 0.5f);

ObjectViewer::ObjectViewer(MeshManager* meshManager)
    :meshManager(meshManager), vMode(noView), color(defaultColor)
{

}

void ObjectViewer::init() {
    restoreStateFromFile();
    startAnimation();

    setGridIsDrawn();
    setAxisIsDrawn(false);
    //setFPSIsDisplayed(true);

    /*glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW); // front face -> clock wise enumeration*/

    this->camera()->setPosition(qglviewer::Vec(0.0533285f, 2.13141f, 0.707263f));
    this->camera()->setUpVector(qglviewer::Vec(0.0f, 0.0f, 1.0f));
    this->camera()->lookAt(qglviewer::Vec(0.0f, 0.0f, 0.0f));

    this->setBackgroundColor(QColor(255,255,255));
    resize(800, 600);
}

void ObjectViewer::draw() {
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

     // restore, to correctly show the axis and the fps
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glShadeModel(GL_SMOOTH);
}

void ObjectViewer::drawTriangles() {
    //GLfloat light0_position[] = { -0.5f, 1.0f, 1.0f, 0.0f };
    qglviewer::Vec cameraPosition = camera()->position();
    GLfloat light0_position[] = { cameraPosition[0], cameraPosition[1], cameraPosition[2], 0.0f };
    //GLfloat light1_position[] = { -0.5f, 0.0f, 1.0f, 0.0f };
    GLfloat light_term[] = {0.8f, 0.8f, 0.8f};
    GLfloat light_specular_term[] = { 1.0f, 1.0f, 1.0f};
    GLfloat ambient_term[] = {0.15f, 0.15f, 0.15f, 1.0f };
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_term );
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_term );
    /*glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_term );
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_term );*/
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_term);


    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);

    //draw the floor of the scene
    qglviewer::Vec floorColor(0.12f, 0.42f, 0.23f);

    GLfloat ambient_material_term[] = { floorColor[0], floorColor[1], floorColor[2], 1.0 };
    GLfloat diffuse_material_term[] = { floorColor[0], floorColor[1], floorColor[2], 1.0 };
    GLfloat specular_term[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    //glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_material_term);
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_material_term);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_term);

    /*qglviewer::Vec normalFloor(cameraPosition);
    normalFloor.normalize();

    glNormal3fv(normalFloor);
    glVertex3fv(qglviewer::Vec(-10, 10, 0));
    glVertex3fv(qglviewer::Vec(-10, -10, 0));
    glVertex3fv(qglviewer::Vec(10, -10, 0));

    glNormal3fv(normalFloor);
    glVertex3fv(qglviewer::Vec(100, -100, 0));
    glVertex3fv(qglviewer::Vec(100, 100, 0));
    glVertex3fv(qglviewer::Vec(-100, 100, 0));*/


    // draw the triangle
    const std::vector<qglviewer::Vec * >* triangles = meshManager->getTriangles();
    unsigned int size = triangles->size();

    float normalX(0.0f);
    float normalY(0.0f);
    float normalZ(0.0f);

    ambient_material_term[0] = color[0]; ambient_material_term[1] =  color[1]; ambient_material_term[2] =  color[2];
    diffuse_material_term[0] = color[0]; diffuse_material_term[1] = color[1]; diffuse_material_term[2] = color[2];

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_material_term);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_material_term);

    for( unsigned int i(0); i < size; i = i + 3) {

        // get the three vertices needed to draw a triangle
        // and compute the normal of the triangle
        qglviewer::Vec vertex1 = *(*triangles)[i];
        qglviewer::Vec vertex2 = *(*triangles)[i+1];
        qglviewer::Vec vertex3 = *(*triangles)[i+2];

        if(((vertex1 - vertex2).norm() > 0.0001f)
                && ((vertex1 - vertex3).norm() > 0.0001f)
                && ((vertex2 - vertex3).norm() > 0.0001f)) {

            qglviewer::Vec vector1 = vertex3 - vertex1;
            qglviewer::Vec vector2 = vertex2 - vertex1;

            // compute the normal
            Utils::crossProduct(vector1[0], vector1[1], vector1[2],
                    vector2[0], vector2[1], vector2[2],
                    normalX, normalY, normalZ);

            qglviewer::Vec normal(normalX, normalY, normalZ);
            normal.normalize();

            // draw the triangle
            glNormal3fv(normal);
            glVertex3fv(vertex1); //Utiliser color

            glNormal3fv(normal);
            glVertex3fv(vertex2);

            glNormal3fv(normal);
            glVertex3fv(vertex3);
        }
    }

    glEnd();

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

void ObjectViewer::drawPoints() {
     glPointSize(2.0f);
     glBegin(GL_POINTS);

     const std::vector< qglviewer::Vec * >* points = meshManager->getPoints();

     unsigned int size = points->size();
     for( unsigned int i(0); i < size; ++i) {

         // draw the points
         glColor3fv(color);
         glVertex3fv(*(*points)[i]);
     }

     glEnd();
}

void ObjectViewer::closeEvent(QCloseEvent *event) {
    emit closeSignal();
    event->accept();
}

void ObjectViewer::wireframeMode() {
    vMode = wireframe;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}

void ObjectViewer::flatMode() {
    vMode = flat;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}


void ObjectViewer::smoothMode() {
    vMode = smooth;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}

void ObjectViewer::pointMode() {
    vMode = point;

    // tell the meshManager to generate new point/triangle
    meshManager->setUpdateOnMesh();
}

void ObjectViewer::noViewMode() {
    vMode = noView;
}
