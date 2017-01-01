#include <QApplication>

#include "mainwindow.h"
#include "tabletapplication.h"
#include "tabletcanvas.h"

int main(int argc, char *argv[])
{
    TabletApplication a(argc, argv);
    //QApplication a(argc, argv);
    MainWindow w;
    a.setCanvas((TabletCanvas*)w.centralWidget()->window());
    w.show();

    return a.exec();
}
