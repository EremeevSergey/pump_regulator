#ifndef PUMPING_STATION_H
#define PUMPING_STATION_H

#include "widgets/pump_widget.h"
#include "widgets/water_tank.h"

namespace GUI {

class CPumpingStation : public QWidget
{
    Q_OBJECT
public:
    explicit CPumpingStation(QWidget *parent = nullptr);
    void switchOn      ()         {Pump_->switchOn ();}
    void switchOff     ()         {Pump_->switchOff();}
    bool getSensorState(int index){return Tank_->getSensorState(index);}
private:
    CPumpWidget *Pump_;
    CWaterTank  *Tank_;
signals:
    void sensorChange(int level,bool state);
public slots:
    void doWork   ();
};

}//namespace GUI
#endif // PUMPING_STATION_H
