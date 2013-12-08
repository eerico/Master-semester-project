#include "SimplificationScene.h"

SimplificationScene::SimplificationScene(MeshManager* meshManager, std::vector< Curve* >* curveArray)
    :QGraphicsScene(), meshManager(meshManager), curveArray(curveArray)
{

}
SimplificationScene::~SimplificationScene() {

}
