#ifndef PROFILEMERGESCENE_H
#define PROFILEMERGESCENE_H

#include <vector>
#include "../MeshManager.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

/**
 * @brief The ProfileMergeScene class
 * This class is a scene used to allow the user to merge different profile.
 * This scene load 1 profile and the user can choose which vertices to keep
 * from this profile when doing the merge
 */
class ProfileMergeScene : public QGraphicsScene
{
public:
    /**
     * @brief ProfileMergeScene
     * @param newProfile a vector containing all vertices that we want to kkep from this profile
     * @param meshManager
     * @param firstScene Used to know if we must load the profile of the currently selected edge
     * or the previously selected edge
     */
    ProfileMergeScene(std::vector<Vertex*>* newProfile, MeshManager* meshManager, bool firstScene);

    /**
     * @brief updateProfileSelected
     * When the user select another edge, we must update the scene to load the new selected edge
     */
    void updateProfileSelected();

    /**
     * @brief revert
     * If some changes (color, brush) has been made on the vertices and the edges
     * this method restore to their inital value
     */
    void revert();

    /**
     * @brief getProfileToMerge
     * @return the profile the we are currently merging
     */
    Profile* getProfileToMerge();

private:
    /**
     * @brief loadProfile
     * load the profile in this scene based on the currently selected edge
     */
    void loadProfile();

    /**
     * @brief mousePressEvent
     * Used to select which vertices will be kept from this profile during the merge
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

    MeshManager* meshManager;

    // the new profile made of the selected vertices of the loaded profile
    std::vector<Vertex*>* newProfile;

    // used to know if this scene is for the first or the second profile
    bool firstScene;

    // the loaded profile
    Profile* profile;    

    // old pen and brush information
    QPen oldEllipsePen;
    QBrush oldEllipseBrush;
    QPen oldLinePen;

    // all profiles that we have change their color/brush
    // and we must revert to their initial state
    std::vector<Profile*> profileToRevertColor;
};

#endif // PROFILEMERGESCENE_H
