#include <QLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simulator.h"

namespace GUI {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Simulator = new CSimulator();
    setCentralWidget(Simulator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

}//namespace GUI
