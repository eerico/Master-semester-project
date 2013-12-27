#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <QSet>
#include <QMessageBox>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <utility>
#include <QMap>

class Profile;
class MeshManager;
class Edge;
class Vertex;


/**
 * @brief The Utils class
 * This class is an utility class that contains differents usefull methods
 */
class Utils
{
private:
    const static char * startXML_Element;
public:
    Utils();

    /**
     * @brief distance
     * Compute the distance between two points (x1, y1) and (x2, y2)
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return The distance between (x1, y1) and (x2, y2)
     */
    static float distance(float x1, float y1, float x2, float y2);

    /**
     * @brief adjustCoordinates3DToScene
     * Transform the coordinates from the 3D coordinates into the scene coordinates
     * (only the x and y coordinates)
     * @param x
     * @param y
     * @param width The scene width
     * @param height The scene height
     */
    static void adjustCoordinates3DToScene(float& x, float& y, float width, float height);

    /**
     * @brief adjustCoordinatesSceneTo3D
     * Transform the coordinates from the scene into the 3D coordinates coordinates
     * (only the x and y coordinates)
     * @param x
     * @param y
     * @param width
     * @param height
     */
    static void adjustCoordinatesSceneTo3D(float& x, float& y, float width, float height);

    /**
     * @brief normalize
     * Normalize the 2D vector defined by (n_x, n_y)
     * @param n_x
     * @param n_y
     */
    static void normalize(float& n_x, float& n_y);

    /**
     * @brief normalize
     * Normalize the 3D vector defined by (n_x, n_y, n_z)
     * @param n_x
     * @param n_y
     * @param n_z
     */
    static void normalize(float& n_x, float& n_y, float& n_z);

    /**
     * @brief dotProduct
     * Compute the dot product between (x1, y1) and (x2, y2)
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return the dot product between (x1, y1) and (x2, y2)
     */
    static float dotProduct(float x1, float y1, float x2, float y2);

    /**
     * @brief getZValueForeground
     * @return The Z value used to draw on the foreground of the scene
     */
    static float getZValueForeground();

    /**
     * @brief getZValueBackground
     * @return The Z value used to draw on the background of the scene
     */
    static float getZValueBackground();

    /**
     * @brief crossProduct
     * Compute the cross product between the vector (x1, y1, z1) and (x2, y2, z2)
     * @param x1
     * @param y1
     * @param z1
     * @param x2
     * @param y2
     * @param z2
     * @param x The resulting x
     * @param y The resulting y
     * @param z The resulting z
     */
    static void crossProduct(float x1, float y1, float z1,
                             float x2, float y2, float z2,
                             float& x, float& y, float& z);

    /**
     * @brief CreateXMLFile, generate a pseudo xml file (since the order must strictly respect this format), that represent the 3d object managed by meshmanager
     * @param meshmanager: the manager containig the 3d model to save
     * @param filename: location where you want to save the file
     */
    static void CreateXMLFile(MeshManager* meshmanager, QString &filename);

    /**
     * @brief readXML: read from a pseudo xml file (see CreateXMLFile) a 3d object that will be managed by meshmanager
     * @param meshmanager the object that will manage the 3d object
     * @param filename location where the xml file reside
     */
    static void readXML(MeshManager* meshmanager, QString &filename);


};

#endif // UTILS_H
