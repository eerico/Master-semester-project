#include "Utils.h"
#include "Geometry/Edge.h"
#include "Geometry/Profile.h"
#include "Geometry/Vertex.h"
#include "MeshManager.h"

const char * Utils::startXML_Element = "ExtrusionModel";

Utils::Utils()
{
}

float Utils::distance(float x1, float y1, float x2, float y2) {
    float diffX = x1 - x2;
    float diffY = y1 - y2;
    return std::sqrt(diffX * diffX + diffY * diffY);
}

void Utils::adjustCoordinates3DToScene(float& x, float& y, float width, float height) {

    width = width / 10.0f;
    height = height / 10.0f;
    // from 3D to Scene coord
    float width_2 = width/ 2.0f;
    float height_2 = height / 2.0f;

    x = x * width_2;
    y = y * width_2;

    y = /*height_2*/ - y + 10.0f * height_2;
    x = x + /*width_2*/ 10.0f * width_2;
}

void Utils::adjustCoordinatesSceneTo3D(float& x, float& y, float width, float height) {
    // from scene coord to 3D
    float width_2 = width/ 2.0f;
    float height_2 = height / 2.0f;

    y = height_2  - y;
    x = -width_2 + x;

    x = x/width_2 * 10.0f;
    y = y/width_2 * 10.0f;
}

void Utils::normalize(float &n_x, float &n_y) {
    float distTmp = n_x * n_x + n_y * n_y;
    if (distTmp != 0.0f) {
        distTmp = sqrt(distTmp);
        n_x = n_x / distTmp;
        n_y = n_y / distTmp;
    }
}

void Utils::normalize(float &n_x, float &n_y, float &n_z) {
    float distTmp = n_x * n_x + n_y * n_y + n_z * n_z;
    if (distTmp != 0.0f) {
        distTmp = sqrt(distTmp);
        n_x = n_x / distTmp;
        n_y = n_y / distTmp;
        n_z = n_z / distTmp;
    }
}

float Utils::dotProduct(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
}

float Utils::getZValueForeground() {
    return 1.0f;
}

float Utils::getZValueBackground() {
    return 0.0f;
}

void Utils::crossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float& x, float& y, float& z) {
    x = y1 * z2 - z1 * y2;
    y = z1 * x2 - x1 * z2;
    z = x1 * y2 - y1 * x2;
}

void Utils::CreateXMLFile(MeshManager* meshmanager, QString &filename){

    QFile file(filename);
    //open a file
    if (!file.open(QIODevice::WriteOnly))
    {
        //show error message if not able to open file
        QMessageBox::warning(0, "Read only", "The file is in read only mode");
    }
    else
    {
        QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
        xmlWriter->setDevice(&file);
        xmlWriter->writeStartDocument();

        //start writing the xml with this general tag:
        xmlWriter->writeStartElement(startXML_Element);


        QSet<QColor*> profiles;
        Vertex* firstVertex =  meshmanager->getFloorPlan();
        Vertex* tempVertex = firstVertex;

        //first write all the profiles a profile is identified by is color:
        do {
            //check if the profile as alredy been written:
            if(!(profiles.contains( tempVertex->getEdge2()->getProfile()->getProfileColorIdentification()))){
                //write the profile if not already written on file:
                tempVertex->getEdge2()->getProfile()->writeXML(xmlWriter);
                //add this prifile to the list containing all written profiles
                profiles.insert( tempVertex->getEdge2()->getProfile()->getProfileColorIdentification());
            }
            tempVertex = tempVertex->getNeighbor2();
        }while(tempVertex != firstVertex);

        //write all floorplan Edges (since the other are useless)
        do {//notice that tempVertex == firstVertex because of the complete round done in the profile writing
            tempVertex->getEdge2()->writeXML(xmlWriter);
            tempVertex = tempVertex->getNeighbor2();
        }while(tempVertex != firstVertex);


        //write the general end tag 3DExtrusionModel
        xmlWriter->writeEndElement();

        xmlWriter->writeEndDocument();
        file.close();
        delete xmlWriter;
    }
}

void Utils::readXML(MeshManager* meshmanager, QString &filename){

    QFile file(filename);
    //If we can't open it, show an error message.
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(0,
                              "parsing XML error",
                              "Couldn't open the file",
                              QMessageBox::Ok);
        return;
    }

    QXmlStreamReader xml(&file);

    Edge* firstEdge =0;
    Edge* tempEdge =0;
    Edge* tempEdge2 =0;
    QMap<QString,Profile*> profiles;

    bool error = false;
    //parse the xml till the end of the file:
    while(!xml.atEnd() &&
          !xml.hasError() &&!error) {
        //Read next element.
        QXmlStreamReader::TokenType token = xml.readNext();
        //If token is just StartDocument, we can skip it
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if(token == QXmlStreamReader::StartElement) {
            //if token is  the StartElement, check if it is the correct one
            if(xml.name() == startXML_Element) {
                continue;
                //if it is a profile we know how to handle it
            } else if (xml.name() == "Profile") {
                std::pair<QString, Profile*> tempProfile = Profile::readXML(xml);
                if(tempProfile.first!=0){
                    profiles.insert(tempProfile.first, tempProfile.second);
                } else {
                    error = true;
                }
                continue;
                //if it is an edge we know how to handle it  and since we know that profiles
                //are wirtten before edges we can already search for the one to associate to thsi edge
            } else if (xml.name() == "Edge"){
                std::pair<QString, Edge*> edgePair = Edge::readXML(xml);
                if(edgePair.second ==0){
                    error = true;
                    continue;
                }
                meshmanager->incrementFloorPlanSize();
                if (firstEdge ==0){
                    firstEdge=  edgePair.second;
                    tempEdge = firstEdge;
                } else{
                    tempEdge2=  edgePair.second;
                    //check if we found a neighbour, if it is the case we can remove a copy of the vertex in common and update all information
                    if(tempEdge2->getVertex1()->getX() == tempEdge->getVertex2()->getX()
                            && tempEdge2->getVertex1()->getY() == tempEdge->getVertex2()->getY()){

                        delete tempEdge2->getVertex1();

                        tempEdge2->setVertex1(tempEdge->getVertex2());

                        tempEdge->getVertex2()->setNeighbor2(tempEdge2->getVertex2());
                        tempEdge->getVertex2()->setNeighbor1(tempEdge->getVertex1());

                        tempEdge->getVertex2()->setEdge1(tempEdge);
                        tempEdge->getVertex2()->setEdge2(tempEdge2);

                        tempEdge = tempEdge2;
                    } else {
                        error = true;
                    }
                }

                //add the profile to the edge since we already read all profiles before eges (since they must all be written at the beginning of the xml):
                QMap<QString,Profile*>::const_iterator i = profiles.find(edgePair.first);
                Profile* a = profiles.value(edgePair.first, 0);
                if(a!=0){
                    tempEdge->setProfile(a);
                } else{
                    error = true;
                }
                continue;
                //we don't know what we are reading hence we can stop
            } else {
                error = true;
            }
        }
    }

    if(!xml.hasError() && !error) { //there are no errors till now
        //check if last edge is linked to the first, if it is the case remove a copy of the shared vertex and upadate all information
        if(firstEdge->getVertex1()->getX() == tempEdge->getVertex2()->getX()
                && firstEdge->getVertex1()->getY() == tempEdge->getVertex2()->getY()){

            delete firstEdge->getVertex1();

            firstEdge->setVertex1(tempEdge->getVertex2());
            firstEdge->getVertex2()->setNeighbor1(firstEdge->getVertex1());

            tempEdge->getVertex2()->setNeighbor2(firstEdge->getVertex2());
            tempEdge->getVertex2()->setNeighbor1(tempEdge->getVertex1());


            tempEdge->getVertex2()->setEdge1(tempEdge);
            tempEdge->getVertex2()->setEdge2(firstEdge);

        } else {
            error = true;
        }
        if (!error){
            //we are sure we have a complete model in memory hence we can instantiate it:
            meshmanager->setFloorPlan(firstEdge->getVertex1());
            meshmanager->setCurrentProfile(profiles.begin().value());

            //add all profiles in the destructormanager:
            QMap<QString, Profile *>::iterator iteratorProfiles = profiles.begin();
            while (iteratorProfiles != profiles.end()) {
                ProfileDestructorManager::putProfile(iteratorProfiles.value());
                iteratorProfiles++;
            }
            //show the model:
            meshmanager->emitNewFloorPlan();
        }
    }

    //remember to clear/close the file and xml vars:
    file.close();
    xml.clear();

    //check if there was some problem: Error handling.
    if(xml.hasError() || error){
        QMessageBox::critical(0,
                              "Error parsing XML",
                              xml.errorString(),
                              QMessageBox::Ok);
    }

}
