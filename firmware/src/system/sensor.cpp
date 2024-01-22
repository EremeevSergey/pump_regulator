#include "config.h"
#include "hw_specific.h"
#include "sensor.h"

namespace MCU{

CSensor::CSensor(TPort m_Port, TBit m_PortBit)
    : port_  {m_Port,m_PortBit}
    , type_  {NormalOpen}
    , enable_{true}
    , state_ {EPortState::Off}
{
}

void CSensor::invertType()
{
    if (type_==NormalOpen) type_=NormalClosed;
    else                   type_=NormalOpen;
}

void CSensor::init()
{
    port_.init(PORT_INPUT,PORT_PULL_UP);
    type_ = NormalOpen;
}

void CSensor::tick()
{
    if (enable_){
        EPortState st = port_.get();
        if (type_==NormalClosed) state_ = st;
        else                     state_ = !st;
    }
    else state_ = EPortState::Off;
}

EPortState CSensor::getState()
{
    if (enable_) return state_;
    return EPortState::Off;
}

}// namespace MCU
