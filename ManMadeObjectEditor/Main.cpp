#include "FloorAndProfileViewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FloorAndProfileViewer fpViewer;
    fpViewer.show();

    return app.exec();
}
