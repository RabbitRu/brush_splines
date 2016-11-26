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
    QString path = QDir::currentPath() + "/untitled.png";
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file for the record"),
                                                    QDir::currentPath());
    if (!m_canvas->recordStroke(fileName))
        QMessageBox::information(this, "Error Opening File",
                                 "Could not open file");
}

void MainWindow::on_actionPlay_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file to play"),
                                                    QDir::currentPath());
    if (!m_canvas->playStroke(fileName))
        QMessageBox::information(this, "Error Opening File",
                                 "Could not open file");
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
