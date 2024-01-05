#include "relay.h"

namespace MCU{

void CRelay::init()
{
    m_Port.init(PORT_OUTPUT_OD,PORT_NO_PULL);
    m_OldState       = EPortState::Off;
    m_HysteresisMsec = 0;
    m_Countdown      = 0;
}

void CRelay::tick()
{
    if (m_Countdown>0) --m_Countdown;
}

void CRelay::switchOn ()
{
    if (m_OldState!=On && m_Countdown<=0){
        m_OldState = On;
        m_Countdown = m_HysteresisMsec;
        m_Port.set();
    }
}

void CRelay::switchOff()
{
    if (m_OldState!=Off && m_Countdown<=0){
        m_OldState = Off;
        m_Countdown = m_HysteresisMsec;
        m_Port.reset();
    }
}

}// namespace MCU
