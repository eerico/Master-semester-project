#ifndef PROFILEMERGEWINDOW_H
#define PROFILEMERGEWINDOW_H

#include <QDialog>
#include "ProfileMergeScene.h"
#include "BasicQGraphicsView.h"
#include <QGridLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "../MeshManager.h"
#include "../GarbageCollector/ProfileDestructorManager.h"


class ProfileMergeWindow: public QDialog
{
    Q_OBJECT
public:
    ProfileMergeWindow(MeshManager* meshManager);
    ~ProfileMergeWindow();

public slots:
    /**
     * @brief merge
     * merge the two selected profiles based on the selected vertices
     * of the two profiles
     */
    void merge();

    /**
     * @brief cancel
     * cancel the merging
     */
    void cancel();

    /**
     * @brief updateScenes
     * updated the two scene when the user select another edge
     */
    void updateScenes();

signals:
    /**
     * @brief closeSignal
     * Signal emitted when the window is closed
     */
    void closeSignal();

private:
    // the first scene loading the profile associated with the currently selected edge
    ProfileMergeScene* scene;
    // the second scene loading the profile associated with the previously selected edge
    ProfileMergeScene* scene2;

    // the two associated view
    BasicQGraphicsView* view;
    BasicQGraphicsView* view2;

    // all layouts
    QGridLayout* gridLayout;
    QHBoxLayout* hBoxLayout;
    QHBoxLayout* hBoxLayout2;

    // the merge button that will merge the two profile
    QPushButton* mergeButton;
    // the cancel button that will cancel the merging
    QPushButton* cancelButton;

    MeshManager* meshManager;

    // a vector containing all vertices that will be used
    // to merge the two profiles
    std::vector<Vertex* > newProfile;

    void closeEvent(QCloseEvent* event);

    /**
     * @brief The VertexComparator struct
     * A comparator used to sort the newProfile vector
     */
    struct VertexComparator
    {
        bool operator()(Vertex* vertex1, Vertex* vertex2){
            return vertex1->getY() < vertex2->getY();
        }
    };

    // used to know if the user has pressed one of the two buttons
    bool buttonPressed;

    void keyPressEvent(QKeyEvent *e);
};

#endif // PROFILEMERGEWINDOW_H
