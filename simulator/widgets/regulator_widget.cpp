#include "regulator_widget.h"
#include "ui_regulator_widget.h"
#include "utils.h"

namespace GUI {

CRegulatorWidget::CRegulatorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::regulator_widget)
    , SensorLed1_{nullptr}
    , SensorLed2_{nullptr}
    , WorkingLed_{nullptr}

{
    ui->setupUi(this);
    SensorLed1_ = new CRgbLedWidget();
    SensorLed2_ = new CRgbLedWidget();
    WorkingLed_ = new CRgbLedWidget();
    setWidgetTo(SensorLed1_,ui->wSensor1);
    setWidgetTo(SensorLed2_,ui->wSensor2);
    setWidgetTo(WorkingLed_,ui->wLedMain);

//    connect(ui->btSensor1,SIGNAL(pressed ()),this,SIGNAL(button1Pressed ()));
//    connect(ui->btSensor1,SIGNAL(released()),this,SIGNAL(button1Released()));
//    connect(ui->btSensor2,SIGNAL(pressed ()),this,SIGNAL(button2Pressed ()));
//    connect(ui->btSensor2,SIGNAL(released()),this,SIGNAL(button2Released()));
    updateUi();
}

CRegulatorWidget::~CRegulatorWidget()
{
    delete ui;
}

void CRegulatorWidget::updateUi()
{
    bool en = ui->btPower->isChecked();
    ui->regulatorFrame->setEnabled(en);
    if (en) ui->btPower->setText("Вкл.\n");
    else    ui->btPower->setText("\nВыкл.");

    if (ui->btSensor1->isDown() || ui->btSensor1->isChecked())
          ui->btSensor1->setText("Вкл.\n");
    else  ui->btSensor1->setText("\nВыкл.");

    if (ui->btSensor2->isDown() || ui->btSensor2->isChecked())
          ui->btSensor2->setText("Вкл.\n");
    else  ui->btSensor2->setText("\nВыкл.");
}

void CRegulatorWidget::on_btPower_toggled(bool checked)
{
    updateUi();
    if (checked) emit powerOn();
    else         emit powerOff();
}

void CRegulatorWidget::on_btSensor1_toggled(bool){ updateUi();}
void CRegulatorWidget::on_btSensor2_toggled(bool){ updateUi();}


bool CRegulatorWidget::getButton1State (){
    return ui->btSensor1->isDown() || ui->btSensor1->isChecked();
}

bool CRegulatorWidget::getButton2State (){
    return ui->btSensor2->isDown() || ui->btSensor2->isChecked();
}





}//namespace GUI

