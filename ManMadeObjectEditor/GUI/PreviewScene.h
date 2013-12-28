#ifndef PREVIEWSCENE_H
#define PREVIEWSCENE_H

#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include "../Geometry/Vertex.h"

/**
 * @brief The PreviewScene class
 * This class is a scene used to show preview before applying algorithm
 */
class PreviewScene : public QGraphicsScene
{
public:
    PreviewScene();

    /**
     * @brief loadPreview
     * Load the preview
     * @param vertexToShow the vertices to show with the edges
     */
    void loadPreview(std::vector<Vertex*>* vertexToShow);

private:
    // The radius of the vertices that we draw
    static const int vertexRadius;
};

#endif // PREVIEWSCENE_H
