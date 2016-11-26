#ifndef TABLETAPPLICATION_H
#define TABLETAPPLICATION_H

#include <QApplication>

#include "tabletcanvas.h"

class TabletApplication : public QApplication
{
    Q_OBJECT

public:
    TabletApplication(int &argv, char **args)
        : QApplication(argv, args) {}

    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void setCanvas( TabletCanvas *canvas)
    { m_canvas = canvas; }

private:
    TabletCanvas *m_canvas;
};

#endif
