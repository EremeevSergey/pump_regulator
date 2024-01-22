#include "button.h"

namespace MCU{

void CButton::init()
{
    m_Port.init(PORT_INPUT,PORT_PULL_UP);
}

void CButton::tick()
{
    auto state = getState();
    if (m_OldState!=state){
        if (state==Release) {
            m_Click = !(m_Countup==0) && !isLongPress();
            m_Countup=0;
        }
        else
            m_Countup=1;
    }
    else if (m_OldState==Press){
        if (m_Countup>0) {
            m_Countup++;
        }
    }
    m_OldState = state;
}


}//namespace MCU
