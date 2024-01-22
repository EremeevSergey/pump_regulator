#include "rgb_led.h"

namespace MCU{

CRgbLed::CRgbLed(int red_pin,int green_pin,int blue_pin)
    : portR_{HW_PinToPortdata(red_pin)}
    , portG_{HW_PinToPortdata(green_pin)}
    , portB_{HW_PinToPortdata(blue_pin)}
    , portRSequence_{portR_}
    , portGSequence_{portG_}
    , portBSequence_{portB_}
{
}

void CRgbLed::init()
{
    portRSequence_.init();
    portGSequence_.init();
    portBSequence_.init();
}

void CRgbLed::tick()
{
    portRSequence_.tick();
    portGSequence_.tick();
    portBSequence_.tick();
}

}// namespace MCU
