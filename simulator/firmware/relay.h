#ifndef RELAY_H
#define RELAY_H
#include "digit_port.h"

namespace MCU{

class CRelay
{
public:
    CRelay(TPortBit m_PortBit):CRelay(m_PortBit.Port,m_PortBit.Bit){}
    CRelay(TPort m_Port, TBit m_PortBit):m_Port{m_Port,m_PortBit},m_OldState{Off},m_HysteresisMsec{0},m_Countdown{0}{}
    void init();
    void tick();
    void setHysteresisTime(int msec){m_HysteresisMsec = msec;}
    void switchOn ();
    void switchOff();
private:
    CDigitPort m_Port;
    EPortState m_OldState;
    int        m_HysteresisMsec;
    int        m_Countdown;
};

}// namespace MCU

#endif // RELAY_H
