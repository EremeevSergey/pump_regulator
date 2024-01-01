#ifndef RGB_LED_H
#define RGB_LED_H
#include "digit_port.h"

namespace MCU{

class CRgbLed
{
public:
    CRgbLed(int red_pin,int green_pin,int blue_pin);
    void init();
    void tick();

    void setSequence (const TPortSequenceItem* r,
                      const TPortSequenceItem* g,
                      const TPortSequenceItem* b){
        portRSequence_.setSequence(r);
        portGSequence_.setSequence(g);
        portBSequence_.setSequence(b);
    }
    void setRSequence(const TPortSequenceItem* sequence){portRSequence_.setSequence(sequence);}
    void setGSequence(const TPortSequenceItem* sequence){portGSequence_.setSequence(sequence);}
    void setBSequence(const TPortSequenceItem* sequence){portBSequence_.setSequence(sequence);}

    bool isRSequenceFinished(){return portRSequence_.isSequenceFinished();}
    bool isGSequenceFinished(){return portGSequence_.isSequenceFinished();}
    bool isBSequenceFinished(){return portBSequence_.isSequenceFinished();}

    void redOff  (){portRSequence_.off();}
    void redOn   (){portRSequence_.on ();}
    void greenOff(){portGSequence_.off();}
    void greenOn (){portGSequence_.on ();}
    void blueOff (){portBSequence_.off();}
    void blueOn  (){portBSequence_.on ();}

    void set(EPortState red,EPortState green,EPortState blue){
        portRSequence_.set(red);portGSequence_.set(green);portBSequence_.set(blue);
    }
private:
    CDigitPort portR_;
    CDigitPort portG_;
    CDigitPort portB_;

    CPortSequence portRSequence_;
    CPortSequence portGSequence_;
    CPortSequence portBSequence_;
};

}// namespace MCU

#endif // RGB_LED_H
