#include <QVBoxLayout>
#include "pumping_station.h"

namespace GUI {

CPumpingStation::CPumpingStation(QWidget *parent)
    : QWidget{parent}
    , Pump_{nullptr}
    , Tank_{nullptr}
    , WaterCheckBox{nullptr}
{
    Pump_ = new CPumpWidget();
    Tank_ = new CWaterTank();
    Tank_->addSensor({10});
    Tank_->addSensor({62});
    Tank_->setWaterDelta(0);

    WaterCheckBox = new QCheckBox();

    QVBoxLayout* lay = new QVBoxLayout();
    lay->addWidget(WaterCheckBox,0);
    lay->addWidget(Tank_,1);
    lay->addWidget(Pump_,0);
    setLayout(lay);

    connect(Pump_,SIGNAL(doWork())              ,this,SLOT  (doWork()));
    connect(Tank_,SIGNAL(sensorChange(int,bool)),this,SIGNAL(sensorChange(int,bool)));
    connect(WaterCheckBox,SIGNAL(stateChanged(int)),this,SLOT(waterOnOff()));

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

}

void CPumpingStation::waterOnOff()
{
    if (WaterCheckBox->isChecked())
        Tank_->setWaterDelta(1);
    else
        Tank_->setWaterDelta(0);
    updateUi();
}
}//namespace GUI
