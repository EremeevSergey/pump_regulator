#ifndef SENSOR_H
#define SENSOR_H

#include "digit_port.h"

namespace MCU{

class CSensor
{
public:
    enum EType{
        NormalOpen,
        NormalClosed,
    };

    CSensor(TPortBit m_PortBit):CSensor(m_PortBit.Port,m_PortBit.Bit){}
    CSensor(TPort m_Port,TBit m_PortBit);
    void init();
    void tick();

    EType getType() const {return type_;}
    void  invertType();
    void  enable () {enable_=true;}
    void  disable() {enable_=false;}

    /*!
     * \brief getState
     * \return
     */
    EPortState  getState();
private:
    CDigitPort port_;
    EType      type_;
    bool       enable_;
    EPortState state_;
};

}// namespace MCU

#endif // SENSOR_H
