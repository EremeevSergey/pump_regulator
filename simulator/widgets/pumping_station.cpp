#include <QVBoxLayout>
#include "pumping_station.h"

namespace GUI {

CPumpingStation::CPumpingStation(QWidget *parent)
    : QWidget{parent}
    , Pump_{nullptr}
    , Tank_{nullptr}
{
    Pump_ = new CPumpWidget();
    Tank_ = new CWaterTank();
    Tank_->addSensor({10});
    Tank_->addSensor({62});

    QVBoxLayout* lay = new QVBoxLayout();
    lay->addWidget(Tank_,1);
    lay->addWidget(Pump_,0);
    setLayout(lay);

    connect(Pump_,SIGNAL(doWork())              ,this,SLOT  (doWork()));
    connect(Tank_,SIGNAL(sensorChange(int,bool)),this,SIGNAL(sensorChange(int,bool)));
}

void CPumpingStation::doWork ()
{
    auto delta = Tank_->getWaterDelta();
    Tank_->addWaterDelta(-delta*2);
}

}//namespace GUI
