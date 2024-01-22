#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "widgets/regulator_widget.h"
#include "widgets/pumping_station.h"
#include "widgets/veeprom.h"

namespace GUI {

class CSimulator:public QWidget
{
    Q_OBJECT
public:
    CSimulator();
    ~CSimulator();

    void init();

    void HW_SetPortBit   (int bit);
    void HW_ResetPortBit (int bit);
    bool HW_GetPortBit(int bit);

    CVirtualEEPROM EEPROM;
protected:
    virtual void timerEvent(QTimerEvent *event) override;
private:
    CRegulatorWidget *Regulator_;
    CPumpingStation  *PumpingStation_;
    int               SystemTimerId_;
private slots:
    void powerOn();
    void powerOff();
};

}//namespace GUI

extern GUI::CSimulator* Simulator;
#endif // SIMULATOR_H
