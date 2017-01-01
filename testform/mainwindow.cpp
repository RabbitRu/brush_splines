#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tabletcanvas.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_canvas=ui->widget;
    /*ui->actionBasic->setChecked(true);
    ui->actionEnableTransition->setChecked(false);
    ui->actionTransparencyFixed->setChecked(true);
    ui->actionWidthFixed->setChecked(true);*/
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_canvas->resize(this->width(),this->height());
}
void MainWindow::on_actionExit_triggered()
{
    MainWindow::close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About"),
                       tr("Alpha version 0.2"));
}

void MainWindow::on_actionSave_triggered()
{
    QString path = QDir::currentPath() + "/untitled.bmp";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Picture"),
                                                    path);

    if (!m_canvas->saveImage(fileName))
        QMessageBox::information(this, "Error Saving Picture",
                                 "Could not save the image");
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Picture"),
                                                    QDir::currentPath());

    if (!m_canvas->loadImage(fileName))
        QMessageBox::information(this, "Error Opening Picture",
                                 "Could not open picture");
}

void MainWindow::on_actionBrush_triggered()
{
    m_canvas->setColor(QColorDialog::getColor(m_canvas->color()));
}

void MainWindow::on_actionFixed_triggered()
{

}

void MainWindow::on_actionRecord_triggered()
{
    if(ui->actionRecord->isChecked()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                        QDir::currentPath());
        if (!m_canvas->recordStroke(fileName))
            QMessageBox::information(this, "Error Opening File",
                                     "Could not open file");
    }else
        m_canvas->stoprecord();
}

void MainWindow::on_actionPlay_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file to play"),
                                                    QDir::currentPath());
    QPoint ret = m_canvas->playStroke(fileName);
    if(ret.x()==-1)  QMessageBox::information(this, "Error Opening File", "Could not open file");
    else QMessageBox::information(this, "Drawning Time:", "Time: "+QString::number(ret.x())+"\n Count: "+QString::number(ret.y()));
}

void MainWindow::on_actionNew_triggered()
{
    if(!m_canvas->newImage()) QMessageBox::information(this, "Error Creating New Image",
                                                       "Unknown Error");

}

void MainWindow::on_actionStartingColor_triggered()
{
    m_canvas->setStartingColor(QColorDialog::getColor());
}

void MainWindow::on_actionEndingColor_triggered()
{
    m_canvas->setEndingColor(QColorDialog::getColor());
}

void MainWindow::on_actionBasic_triggered()
{
    if(ui->actionBrush_mod->isChecked())
        ui->actionBrush_mod->setChecked(false);
    if(ui->actionSplines->isChecked())
        ui->actionSplines->setChecked(false);
    ui->actionBasic->setChecked(true);
    m_canvas->setAlgo((TabletCanvas::AlgoValuator::BasicVal));
    update();
}

void MainWindow::on_actionBrush_mod_triggered()
{
    if(ui->actionBasic->isChecked())
        ui->actionBasic->setChecked(false);
    if(ui->actionSplines->isChecked())
        ui->actionSplines->setChecked(false);
    ui->actionBrush_mod->setChecked(true);
    m_canvas->setAlgo((TabletCanvas::AlgoValuator::BrushVal));
    update();
}

void MainWindow::on_actionSplines_triggered()
{
    if(ui->actionBrush_mod->isChecked())
        ui->actionBrush_mod->setChecked(false);
    if(ui->actionBasic->isChecked())
        ui->actionBasic->setChecked(false);
    ui->actionSplines->setChecked(true);
    m_canvas->setAlgo((TabletCanvas::AlgoValuator::SplineVal));
    update();
}

void MainWindow::on_actionWidthFixed_triggered()
{
    if(ui->actionWidthPressure->isChecked())
        ui->actionWidthPressure->setChecked(false);
    ui->actionWidthFixed->setChecked(true);
    m_canvas->setWidthType((TabletCanvas::Valuator::FixedValuator));
    update();
}

void MainWindow::on_actionWidthPressure_triggered()
{
    if(ui->actionWidthFixed->isChecked())
        ui->actionWidthFixed->setChecked(false);
    ui->actionWidthPressure->setChecked(true);
    m_canvas->setWidthType((TabletCanvas::Valuator::PressureValuator));
    update();
}

void MainWindow::on_actionTransparencyFixed_triggered()
{
    if(ui->actionTransparencyPressure->isChecked())
        ui->actionTransparencyPressure->setChecked(false);
    ui->actionTransparencyFixed->setChecked(true);
    m_canvas->setTransparencyType((TabletCanvas::Valuator::FixedValuator));
    update();
}

void MainWindow::on_actionTransparencyPressure_triggered()
{
    if(ui->actionTransparencyFixed->isChecked())
        ui->actionTransparencyFixed->setChecked(false);
    ui->actionTransparencyPressure->setChecked(true);
    m_canvas->setTransparencyType((TabletCanvas::Valuator::PressureValuator));
    update();
}

void MainWindow::on_actionTransition_triggered()
{
    if(!ui->actionTransition->isChecked()){
        m_canvas->setTransitionType(TabletCanvas::Valuator::NoValuator);
    }else{
        m_canvas->setTransitionType(TabletCanvas::Valuator::PressureValuator);
    }
    update();

}

void MainWindow::on_actionMax_Width_triggered()
{
    bool ok;
    m_canvas->setWidth(QInputDialog::getDouble(this, tr("Input max brush width"),
                                               tr("Width:"), (double)m_canvas->getWidth(),1, 1000, 3, &ok));
    if(!ok)return;//throw exeption
}

void MainWindow::on_actionMax_Transparency_triggered()
{
    //bool ok;
    m_canvas->setTrans(QInputDialog::getDouble(this, tr("Input max brush transparency"),
                                               tr("Transparency:"), (double)m_canvas->getTrans(), 0, 1, 5));
    //if(!ok)return;//throw exeption
}

void MainWindow::on_actionAmmount_of_repeats_triggered()
{
    //bool ok;
    m_canvas->setAmmount(QInputDialog::getDouble(this, tr("Input ammount of repeats"),
                                               tr("Ammount:"), (double)m_canvas->getAmmount(), 1, 99999));
    //if(!ok)return;//throw exeption
}

void MainWindow::on_actionCompare_triggered()
{
    QString fileName1 = QFileDialog::getOpenFileName(this, tr("Open Picture"),
                                                    QDir::currentPath());
    QString fileName2 = QFileDialog::getOpenFileName(this, tr("Open Picture"),
                                                    QDir::currentPath());
    QImage a,b;
    a.load(fileName1);b.load(fileName2);
    double res = m_canvas->compareImages(a,b);
    QMessageBox::information(this, "Comparsion result", "Match: "+QString::number(res)+"%");
}

void MainWindow::on_actionDebug_Points_triggered()
{
    if(ui->actionDebug_Points->isChecked()){
        m_canvas->setDebugPoints(true);
    }else{
        m_canvas->setDebugPoints(false);
    }
}

void MainWindow::on_actionDeltoid_triggered()
{

    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(Deltoid, file);
}

void MainWindow::on_actionSin_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(Sin,file);
}

void MainWindow::on_actionSqrt_triggered()
{

    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(Sqrt, file);
}

void MainWindow::on_actionTrifolium_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(Trifolium, file);
}

void MainWindow::on_actionCardioid_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(Cardiode, file);
}

void MainWindow::on_actionArch_Cpiral_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(ArchSpiral, file);
}

void MainWindow::on_actionRanunculoid_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                QDir::currentPath());
    m_canvas->drawTestFunc(Ranunculoid, file);
}

void MainWindow::on_actionSetA_triggered()
{
    m_canvas->setA(QInputDialog::getDouble(this, tr("Input a"),
                                               tr("a:"), (double)m_canvas->getA(), 0, 9999, 5));
}

void MainWindow::on_actionSetB_triggered()
{
    m_canvas->setB(QInputDialog::getDouble(this, tr("Input b"),
                                               tr("b:"), (double)m_canvas->getB(), 0, 9999, 5));
}

void MainWindow::on_actionSetSteps_triggered()
{
    m_canvas->setStepmod(QInputDialog::getInt(this, tr("Input stepmod"),
                                               tr("stepmod:"), (double)m_canvas->getStepmod(), 0, 9999, 5));
}

void MainWindow::on_actionNormal_triggered()
{
    if(ui->actionFibonachi->isChecked())
        ui->actionFibonachi->setChecked(false);
    if(ui->actionGeom_2->isChecked())
        ui->actionGeom_2->setChecked(false);
    ui->actionNormal->setChecked(true);
    m_canvas->setSval(Lin);
}

void MainWindow::on_actionFibonachi_triggered()//Now arifm prog
{
    if(ui->actionNormal->isChecked())
        ui->actionNormal->setChecked(false);
    if(ui->actionGeom_2->isChecked())
        ui->actionGeom_2->setChecked(false);
    ui->actionFibonachi->setChecked(true);
    m_canvas->setSval(Arifm);
}

void MainWindow::on_actionGeom_2_triggered()
{
    if(ui->actionFibonachi->isChecked())
        ui->actionFibonachi->setChecked(false);
    if(ui->actionNormal->isChecked())
        ui->actionNormal->setChecked(false);
    ui->actionGeom_2->setChecked(true);
    m_canvas->setSval(Geom);
}

void MainWindow::on_actionGenTests_triggered()
{
    m_canvas->testgen();
}

void MainWindow::on_actionRunTests_triggered()
{
    m_canvas->drawtests();
}

void MainWindow::on_actionCompareResults_triggered()
{
    m_canvas->compareResults();
}
