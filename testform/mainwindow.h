#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tabletcanvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private slots:
    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionBrush_triggered();

    void on_actionFixed_triggered();

    void on_actionRecord_triggered();

    void on_actionPlay_triggered();

    void on_actionNew_triggered();

    void on_actionStartingColor_triggered();

    void on_actionEndingColor_triggered();

    void on_actionBasic_triggered();

    void on_actionBrush_mod_triggered();

    void on_actionSplines_triggered();

    void on_actionWidthFixed_triggered();

    void on_actionWidthPressure_triggered();

    void on_actionTransparencyFixed_triggered();

    void on_actionTransparencyPressure_triggered();

    void on_actionTransition_triggered();

    void on_actionMax_Width_triggered();

    void on_actionMax_Transparency_triggered();

    void on_actionAmmount_of_repeats_triggered();

    void on_actionCompare_triggered();

    void on_actionDebug_Points_triggered();

    void on_actionDeltoid_triggered();

    void on_actionSin_triggered();

    void on_actionSqrt_triggered();

    void on_actionTrifolium_triggered();

    void on_actionCardioid_triggered();

    void on_actionArch_Cpiral_triggered();

    void on_actionRanunculoid_triggered();

    void on_actionSetA_triggered();

    void on_actionSetB_triggered();

    void on_actionSetSteps_triggered();

    void on_actionNormal_triggered();

    void on_actionFibonachi_triggered();

    void on_actionGeom_2_triggered();

    void on_actionGenTests_triggered();

    void on_actionRunTests_triggered();

    void on_actionCompareResults_triggered();

private:
    Ui::MainWindow *ui;
    TabletCanvas *m_canvas;
};

#endif // MAINWINDOW_H
