#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QSpacerItem>
#include <QTimerEvent>
#include <cassert>
#include <iostream>
#include "utils.h"
#include "config.h"
#include "system.h"
#include "simulator.h"

GUI::CSimulator* Simulator{nullptr};

void HW_MkInit(){
    Simulator->init();
}

void HW_PinToPortdata(int pin,MCU::TPort& dst_port,MCU::TBit& dst_bit)
{
    dst_port = nullptr;
    dst_bit = pin;
}

bool HW_InitPortBit (MCU::TPort port, MCU::TBit bit,int config,int pull){
    Q_UNUSED(port);
    Q_UNUSED(bit);
    Q_UNUSED(config);
    Q_UNUSED(pull);
    return true;
}

void HW_SetPortBit   (MCU::TPort, MCU::TBit bit){ Simulator->HW_SetPortBit  (bit);}
void HW_ResetPortBit (MCU::TPort, MCU::TBit bit){ Simulator->HW_ResetPortBit(bit);}
MCU::EPortState HW_GetPortBit   (MCU::TPort, MCU::TBit bit){
    if (Simulator->HW_GetPortBit(bit))
        return MCU::Off;
    return     MCU::On;
}

int HW_MainSystemTimerFlag;

bool HW_TimerInit (){return true;}

void HW_EepromInit  (){Simulator->EEPROM.init();}
void HW_EepromClear (){Simulator->EEPROM.clear();}
void HW_EepromReservAddress([[maybe_unused]]std::uint16_t address){}

bool HW_EepromRead (std::uint16_t address,std::uint16_t& data){
    return Simulator->EEPROM.Read(address,data);
}

bool HW_EepromWrite(std::uint16_t address,std::uint16_t  data){
    return Simulator->EEPROM.Write(address,data);
}

namespace GUI {

CSimulator::CSimulator()
    : QWidget()
    , Regulator_     {nullptr}
    , PumpingStation_{nullptr}
    , SystemTimerId_ {0}
{
    Regulator_      = new CRegulatorWidget();
    PumpingStation_ = new CPumpingStation();

    auto horizontalLayout = new QHBoxLayout();
    auto verticalLayout = new QVBoxLayout();
    auto frameRegulator = new QFrame();
    frameRegulator->setFrameShape(QFrame::Panel);
    frameRegulator->setFrameShadow(QFrame::Raised);
    setWidgetTo(Regulator_,frameRegulator);
    verticalLayout->addWidget(frameRegulator);

    auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    horizontalLayout->addLayout(verticalLayout);

    auto frameTank = new QFrame();
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    frameTank->setSizePolicy(sizePolicy);
    frameTank->setFrameShape(QFrame::Panel);
    frameTank->setFrameShadow(QFrame::Raised);
    setWidgetTo(PumpingStation_,frameTank);
    horizontalLayout->addWidget(frameTank);

    setLayout(horizontalLayout);

    connect(Regulator_,SIGNAL(powerOn()),this,SLOT(powerOn()));
    connect(Regulator_,SIGNAL(powerOff()),this,SLOT(powerOff()));
}

CSimulator::~CSimulator()
{
    if (SystemTimerId_!=0) killTimer(SystemTimerId_);
}

void CSimulator::powerOn()
{
    using namespace MCU;
    if (SystemTimerId_==0) SystemTimerId_=startTimer(MAIN_TIMER/1000); //!< миллисекунды 1000/1000 = 1 мсек
    System.init();
}

void CSimulator::powerOff()
{
    if (SystemTimerId_!=0) killTimer(SystemTimerId_);
    SystemTimerId_ = 0;
}

void CSimulator::init()
{
}

void CSimulator::timerEvent(QTimerEvent *event)
{
    using namespace MCU;
    if (event!=nullptr && event->timerId()==SystemTimerId_){
        HW_MainSystemTimerFlag++;
        System.tick();
    }
}

void CSimulator::HW_SetPortBit  (int bit)
{
    using namespace MCU;
    switch (bit) {
    case SENSOR1_LED_RED_PIN  : return Regulator_->sensor1LedRedOn  ();
    case SENSOR1_LED_GREEN_PIN: return Regulator_->sensor1LedGreenOn();
    case SENSOR1_LED_BLUE_PIN : return Regulator_->sensor1LedBlueOn ();
    case SENSOR2_LED_RED_PIN  : return Regulator_->sensor2LedRedOn  ();
    case SENSOR2_LED_GREEN_PIN: return Regulator_->sensor2LedGreenOn();
    case SENSOR2_LED_BLUE_PIN : return Regulator_->sensor2LedBlueOn ();
    case MAIN_LED_RED_PIN     : return Regulator_->mainLedRedOn     ();
    case MAIN_LED_GREEN_PIN   : return Regulator_->mainLedGreenOn   ();
    case MAIN_LED_BLUE_PIN    : return Regulator_->mainLedBlueOn    ();
    case RELAY_OUT_PIN        : return PumpingStation_->switchOn    ();
    default:
        assert(false);
        break;
    }
}

void CSimulator::HW_ResetPortBit (int bit)
{
    using namespace MCU;
    switch (bit) {
    case SENSOR1_LED_RED_PIN  : return Regulator_->sensor1LedRedOff  ();
    case SENSOR1_LED_GREEN_PIN: return Regulator_->sensor1LedGreenOff();
    case SENSOR1_LED_BLUE_PIN : return Regulator_->sensor1LedBlueOff ();
    case SENSOR2_LED_RED_PIN  : return Regulator_->sensor2LedRedOff  ();
    case SENSOR2_LED_GREEN_PIN: return Regulator_->sensor2LedGreenOff();
    case SENSOR2_LED_BLUE_PIN : return Regulator_->sensor2LedBlueOff ();
    case MAIN_LED_RED_PIN     : return Regulator_->mainLedRedOff     ();
    case MAIN_LED_GREEN_PIN   : return Regulator_->mainLedGreenOff   ();
    case MAIN_LED_BLUE_PIN    : return Regulator_->mainLedBlueOff    ();
    case RELAY_OUT_PIN        : return PumpingStation_->switchOff    ();
    default:
        assert(false);
        break;
    }
}

bool CSimulator::HW_GetPortBit   (int bit)
{
    using namespace MCU;
    switch (bit) {
    case BUTTON1_PIN: return Regulator_->getButton1State();
    case BUTTON2_PIN: return Regulator_->getButton2State();
    case SENSOR1_PIN: return PumpingStation_->getSensorState(0);
    case SENSOR2_PIN: return PumpingStation_->getSensorState(1);
    default:
        assert(false);
        break;
    }
    return false;
}

}//namespace GUI
