#include "GUI/FloorAndProfileViewer.h"
#include <QApplication>

#include "Test.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FloorAndProfileViewer fpViewer;
    fpViewer.show();

    //Test::Reconstruction3DTest();

    return app.exec();
}
