#ifndef PROFILEMERGESCENE_H
#define PROFILEMERGESCENE_H

#include <vector>
#include "../MeshManager.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

class ProfileMergeScene : public QGraphicsScene
{
public:
    ProfileMergeScene(std::vector<Vertex*>* newProfile, MeshManager* meshManager, bool firstScene);

    void updateProfileSelected();
    void revert();
private:
    void loadProfile();
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

    MeshManager* meshManager;
    std::vector<Vertex*>* newProfile;

    bool firstScene;
    Profile* profile;

    void deleteOldProfileItems();

    QPen oldEllipsePen;
    QBrush oldEllipseBrush;
    QPen oldLinePen;
};

#endif // PROFILEMERGESCENE_H
