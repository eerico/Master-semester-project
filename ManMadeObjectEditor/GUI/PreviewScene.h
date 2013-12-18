#ifndef PREVIEWSCENE_H
#define PREVIEWSCENE_H

#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include "../Geometry/Vertex.h"

class PreviewScene : public QGraphicsScene
{
public:
    PreviewScene();
    void loadPreview(std::vector<Vertex*>* vertexToShow);

private:
    // The radius of the vertices that we draw
    static const int vertexRadius;
};

#endif // PREVIEWSCENE_H
