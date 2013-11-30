#include "Profile.h"
#include "Edge.h"
#include <iostream>


Profile::Profile(bool empty): pVertex(0)
{
	pVertex = new Vertex(0,0);
    profileVertexIterator = pVertex;
    pVertex->setNeighbor2(0);
    if (!empty) {
        initProfileSkewedLine(2);
        //initProfileBezier(10);
    }	
    
    profileColorIdentification = new QColor;
    profileColorIdentification->setRgb(std::rand()%256, std::rand()%256, std::rand()%256);
}

Profile::~Profile()
{
    Vertex* next;
    while(pVertex != 0)
    {
        next = pVertex->getNeighbor2();
        if (next != 0) {
            delete pVertex->getEdge2();
        }
        delete pVertex->getEllipse();
        delete pVertex;
        pVertex = next;
    }
    
    delete profileColorIdentification;
}

QColor* Profile::getProfileColorIdentification() {
    return profileColorIdentification;
}

void Profile::initProfileSkewedLine(int numSample) {
    //create the skewed line
    float fromW(0.0f);
    float fromZ(0.0f);
    float toW(0.5f);
    float toZ(0.5f);
    
    float w(0.0f);
    float z(0.0f);
    float increment = 1.0/(float)(numSample - 1);
    
    Vertex* current = pVertex;
    Vertex* previous = 0;
    float t(0.0f);
    
    // take samples on this line
    for(int sampleCounter(0); sampleCounter < numSample; sampleCounter++) {
        w = (1.0f - t) * fromW + t * toW;
        z = (1.0f - t) * fromZ + t * toZ;
        current->setX(w);
        current->setY(z);
        
        current->setNeighbor1(previous);
        if (t != 0) {
            previous->setNeighbor2(current);
            Edge* edge = new Edge(previous, current);
            previous->setEdge2(edge);
            current->setEdge1(edge);
        }
        previous = current;
        
        if (sampleCounter < numSample - 1) {
            current = new Vertex(0,0);
        }
        t += increment;
    }
    
    current->setNeighbor2(0);
}

void Profile::initProfileBezier(int numSample) {
    // create the bezier curve
    float p0X(0.0f);
    float p0Y(0.0f);
    float c0X(0.0f);
    float c0Y(1.0f);
    float p1X(0.5f);
    float p1Y(0.5f);
    float c1X(0.5f);
    float c1Y(0.5f);
    
    float w(0.0f);
    float z(0.0f);
    float increment = 1.0f/(float)(numSample - 1);
    
    Vertex* current = pVertex;
    Vertex* previous = 0;
    float t(0.0f);
    
    // take samples on this curve
    for(int sampleCounter(0); sampleCounter < numSample; sampleCounter++) {
        w = pow(1.0f - t, 3) * p0X + 3.0f * pow(1.0f - t, 2) * t * c0X + 3.0f * (1.0f - t) * t * t * c1X + t * t * t * p1X;
        z = pow(1.0f - t, 3) * p0Y + 3.0f * pow(1.0f - t, 2) * t * c0Y + 3.0f * (1.0f - t) * t * t * c1Y + t * t * t * p1Y;
        
        current->setX(w);
        current->setY(z);
        
        current->setNeighbor1(previous);
        if (t != 0) {
            previous->setNeighbor2(current);
            Edge* edge = new Edge(previous, current);
            previous->setEdge2(edge);
            current->setEdge1(edge);
        }
        previous = current;
        
        if (sampleCounter < numSample - 1) {
            current = new Vertex(0,0);
        }
        
        t += increment;
    }
    
    current->setNeighbor2(0);
}

Vertex* Profile::getProfileVertex() {
    return pVertex;
}

bool Profile::addVertexEnd(Vertex* v){
    // find the last vertex
    Vertex* current = pVertex;
    while (current->getNeighbor2()!= 0){
        current = current->getNeighbor2();
    }

    // check the monotonicity
    if (v->getY() < current->getY()) {
        // then the new vertex is below the last. This breaks the monotonicity
        return false;
    }

    // add the vertex v at the end of the profile
    current->setNeighbor2(v);
    v->setNeighbor1(current);
    Edge* edge = new Edge(current, v);
    current->setEdge2(edge);
    v->setEdge1(edge);
    
    return true;
}

void Profile::addProfileVertex(float w, float z) {
    Vertex* newPvertex = new Vertex(w,z);
    
    if(pVertex!= 0){
        Vertex* current = pVertex;
        
        while(current->getY() < z){
            if(current->getNeighbor2() == 0){
                break;
            }
            current = current->getNeighbor2();
        }
        
        if(current->getNeighbor2() != 0){
            Vertex* previous =  current->getNeighbor1();
            
            newPvertex->setNeighbor1(previous);
            newPvertex->setNeighbor2(current);
            
            current->setNeighbor1(newPvertex);
            if(previous != 0){
                previous->setNeighbor2(newPvertex);
            } else {
                pVertex = newPvertex;
            }
        } else {
            newPvertex->setNeighbor1(current);
            newPvertex->setNeighbor2(current->getNeighbor2());
            current->setNeighbor2(newPvertex);
        }
        
    } else {
        pVertex = newPvertex;
        pVertex->setNeighbor1(0);
        pVertex->setNeighbor2(0);
    }
}

void Profile::vertexDecimation() {
    /*
     * The algorithm works as follow: we take a current vertex, its previous vertex and its next
     * vertex. If this three vertices are on the same line then the current vertex doesnt add any
     * details on the profile and we thus can remove it
     */
    Vertex* next;
    Vertex* previous;
    Vertex* current = pVertex;
    while(current != 0) {
        next = current->getNeighbor2();
        previous = current->getNeighbor1();
        if (next != 0 && previous != 0) {
            float fromPreviousToCurrentX = current->getX() - previous->getX();
            float fromPreviousToCurrentY = current->getY() - previous->getY();
            Utils::normalize(fromPreviousToCurrentX, fromPreviousToCurrentY);
            
            float fromCurrentToNextX = next->getX() - current->getX();
            float fromCurrentToNextY = next->getY() - current->getY();
            Utils::normalize(fromCurrentToNextX, fromCurrentToNextY);
            
            float dotProduct = Utils::dotProduct(fromPreviousToCurrentX, fromPreviousToCurrentY, fromCurrentToNextX, fromCurrentToNextY);
            
            if (std::abs(1.0 - dotProduct) < 0.001f) {
                //we can remove this vertex, because it is on a line
                
                next->setEdge1(previous->replaceNeighbour(current, next) );
                next->setNeighbor1(previous);
                previous->setEdge2(next->getEdge1());
                
                delete current->getEdge1();
                delete current->getEdge2();
                delete current;
            }
        }
        
        current = next;
    } 
}

bool Profile::isEqual(Profile* compareProfile) {
    
    Vertex* compare = compareProfile->getProfileVertex();
    Vertex* current = pVertex;
    while(current != 0 && compare!= 0) {
        // test if the two vertices different
        if((std::abs(compare->getX() - current->getX()) > 0.01f) || (std::abs(compare->getY() != current->getY())> 0.01f)){
            break;
        }
        // they are the same, thus test the next one
        compare = compare->getNeighbor2();
        current = current->getNeighbor2();
    }
    if(current == 0 && compare == 0){
        return true;
    }
    return false;   
}

void Profile::nextDirectionPlan() {
    profileVertexIterator = profileVertexIterator->getNeighbor2();
}

void Profile::resetDirectionPlan() {
    profileVertexIterator = pVertex;
}

Vertex* Profile::getProfileVertexIterator() {
    return profileVertexIterator;
}

void Profile::printProfile() {
    std::cout << "Profile: " << std::endl;
    Vertex* iterator = pVertex;
    while(iterator != 0) {
        std::cout << *iterator << std::endl;
        iterator = iterator->getNeighbor2();
    }

}
void Profile::writeXML(QXmlStreamWriter* xmlWriter){
    xmlWriter->writeStartElement("Profile");
    xmlWriter->writeAttribute("color", profileColorIdentification->toRgb().name());
     Vertex* tempV = pVertex;
    while(tempV!= 0){
        tempV->writeXML(xmlWriter);
        tempV = tempV->getNeighbor2();
    }
    xmlWriter->writeEndElement();
}

std::pair<QString, Profile*> Profile::readXML(QXmlStreamReader &xml){
    /* Let's check that we're really getting an Edge. */
    std::pair<QString, Profile*> pair;
    if(xml.tokenType() != QXmlStreamReader::StartElement &&
            xml.name() == "Profile") {
        return pair;
    }


    Vertex* firstVertex=0;
    Vertex* tempVertex;
    Vertex* tempVertex2;

    /* Let's get the attributes for person */
    QXmlStreamAttributes attributes = xml.attributes();
    /* Let's check that person has id attribute. */
    if(attributes.hasAttribute("color")) {
        pair.first = attributes.value("color").toString();
    }
    /* Next element... */
    xml.readNext();
    /*
     * We're going to loop over the things because the order might change.
     * We'll continue the loop until we hit an EndElement named person.
     */
    while(!(xml.tokenType() == QXmlStreamReader::EndElement &&
                xml.name() == "Profile")) {
            if(xml.tokenType() == QXmlStreamReader::StartElement) {
                /* We've found first name. */
                if(xml.name() == "Vertex") {
                    tempVertex2 = Vertex::readXML(xml);
                    if(firstVertex == 0){
                        firstVertex = tempVertex2;
                        tempVertex = firstVertex;

                    } else {

                        tempVertex->setNeighbor2(tempVertex2);
                        Edge* tedge = new Edge(tempVertex, tempVertex2);
                        tempVertex->setEdge2(tedge);

                        tempVertex2->setNeighbor1(tempVertex);
                        tempVertex2->setEdge1(tedge);
                        tempVertex = tempVertex2;
                    }

                }
            }
            xml.readNext();//read end element
            xml.readNext();
        }
        pair.second = new Profile(true);
        delete pair.second->pVertex;
        pair.second->pVertex = firstVertex;
        pair.second->profileVertexIterator = firstVertex;
        delete pair.second->profileColorIdentification;
        pair.second->profileColorIdentification = new QColor(pair.first);

    return pair;
}
