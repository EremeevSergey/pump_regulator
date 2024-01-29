#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "pumping_station.h"

namespace GUI {

CPumpingStation::CPumpingStation(QWidget *parent)
    : QWidget{parent}
    , Pump_{nullptr}
    , Tank_{nullptr}
    , WaterCheckBox{nullptr}
    , delta_{1}
{
    Pump_ = new CPumpWidget();
    Tank_ = new CWaterTank();
    Tank_->addSensor({10});
    Tank_->addSensor({62});
    Tank_->setWaterDelta(0);

    WaterCheckBox  = new QCheckBox();
    InvertCheckBox = new QCheckBox();
    QHBoxLayout* h_lay = new QHBoxLayout();
    h_lay->addWidget(WaterCheckBox);
    h_lay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    h_lay->addWidget(InvertCheckBox);

    QVBoxLayout* lay = new QVBoxLayout();
    lay->addLayout(h_lay,0);
//    lay->addWidget(WaterCheckBox,0);
    lay->addWidget(Tank_,1);

    InvertSensorsCheckBox = new QCheckBox();
    InvertSensorsCheckBox->setText("Поменять местами датчики");
    InvertSensorsCheckBox->setChecked(false);
    h_lay = new QHBoxLayout();
    h_lay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    h_lay->addWidget(InvertSensorsCheckBox);

    lay->addLayout(h_lay,0);
    lay->addWidget(Pump_,0);
    setLayout(lay);

    connect(Pump_,SIGNAL(doWork())              ,this,SLOT  (doWork()));
    connect(Tank_,SIGNAL(sensorChange(int,bool)),this,SIGNAL(sensorChange(int,bool)));
    connect(WaterCheckBox,SIGNAL(stateChanged(int)),this,SLOT(waterOnOff()));
    connect(InvertCheckBox,SIGNAL(stateChanged(int)),this,SLOT(invertOnOff()));
    connect(InvertSensorsCheckBox,SIGNAL(stateChanged(int)),this,SLOT(invertSensorsOnOff()));

    updateUi();
}

void CPumpingStation::doWork ()
{
    auto delta = Tank_->getWaterDelta();
    if (delta==0) Tank_->addWaterDelta(-CWaterTank::WaterDeltaValue);
    else          Tank_->addWaterDelta(-delta*2);
}

void CPumpingStation::updateUi()
{
    if (WaterCheckBox->isChecked())
        WaterCheckBox->setText("Включено");
    else
        WaterCheckBox->setText("Выключено");

    if (InvertCheckBox->isChecked())
        InvertCheckBox->setText("Вода убавляется");
    else
        InvertCheckBox->setText("Вода прибавляется");
}

void CPumpingStation::waterOnOff()
{
    if (WaterCheckBox->isChecked())
        Tank_->setWaterDelta(delta_);
    else
        Tank_->setWaterDelta(0);
    updateUi();
}

void CPumpingStation::invertOnOff()
{
    if (InvertCheckBox->isChecked()) delta_ = -1;
    else delta_ = 1;
    waterOnOff();
}

void CPumpingStation::invertSensorsOnOff()
{
    Tank_->setReverseSensors(InvertSensorsCheckBox->isChecked());
}

}//namespace GUI
