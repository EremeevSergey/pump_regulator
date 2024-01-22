#ifndef BUTTON_H
#define BUTTON_H
#include "config.h"
#include "digit_port.h"

namespace MCU{

class CButton
{
    constexpr  static int LongTimeMsec = 10*1000/(BUTTONS_PERIOD/MAIN_TIMER); //10 сек
public:
    enum EState{
        Press   = Off,
        Release = On,
    };

    CButton(TPortBit m_PortBit):CButton(m_PortBit.Port,m_PortBit.Bit){}
    CButton(TPort m_Port, TBit m_PortBit):m_Port{m_Port,m_PortBit},
        m_OldState{Release},m_Countup{0},m_Click{false}{}
public:
    void init();
    void tick();

    EState getState   (){
        if (m_Port.get()==Off) return Press;
        return Release;
    }
    bool   isLongPress(){return m_Countup>=LongTimeMsec;}
    bool   isPress    (){return getState()==Press;}
    bool   isClick    (){auto ret = m_Click; m_Click=false; return ret;}
    void   resetCounter(){m_Countup=0;}
private:
    CDigitPort m_Port;
    EState     m_OldState;
    int        m_Countup;
    bool       m_Click;

};

}//namespace MCU

#endif // BUTTON_H
