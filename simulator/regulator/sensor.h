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

    CSensor(int port);
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
