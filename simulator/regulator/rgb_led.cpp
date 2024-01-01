#include "rgb_led.h"

namespace MCU{

CRgbLed::CRgbLed(int red_pin,int green_pin,int blue_pin)
    : portR_{red_pin  }
    , portG_{green_pin}
    , portB_{blue_pin }
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
