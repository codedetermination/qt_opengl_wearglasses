#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(texture1);
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion( 4 );
    format.setMinorVersion( 3 );
    format.setSamples( 3);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();
    return a.exec();
}
