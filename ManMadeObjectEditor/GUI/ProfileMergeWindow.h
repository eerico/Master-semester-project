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
    void merge();
    void cancel();
    void updateScenes();

signals:
    void closeSignal();

private:
    ProfileMergeScene* scene;
    ProfileMergeScene* scene2;

    BasicQGraphicsView* view;
    BasicQGraphicsView* view2;

    QGridLayout* gridLayout;
    QHBoxLayout* hBoxLayout;
    QHBoxLayout* hBoxLayout2;

    QPushButton* mergeButton;
    QPushButton* cancelButton;

    MeshManager* meshManager;

    std::vector<Vertex* > newProfile;

    void closeEvent(QCloseEvent* event);


    struct VertexComparator
    {
        bool operator()(Vertex* vertex1, Vertex* vertex2){
            return vertex1->getY() < vertex2->getY();
        }
    };

    bool buttonPressed;

    void keyPressEvent(QKeyEvent *e);
};

#endif // PROFILEMERGEWINDOW_H
