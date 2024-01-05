#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QSpacerItem>
#include <QTimerEvent>
#include <cassert>
#include "utils.h"
#include "firmware/config.h"
#include "firmware/system.h"
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
        return MCU::On;
    return     MCU::Off;
}

int HW_MainSystemTimerFlag;

bool HW_TimerInit (){return true;}

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
    case SENSOR1_LED_RED_PIN  : Regulator_->sensor1LedRedOn  (); break;
    case SENSOR1_LED_GREEN_PIN: Regulator_->sensor1LedGreenOn(); break;
    case SENSOR1_LED_BLUE_PIN : Regulator_->sensor1LedBlueOn (); break;
    case SENSOR2_LED_RED_PIN  : Regulator_->sensor2LedRedOn  (); break;
    case SENSOR2_LED_GREEN_PIN: Regulator_->sensor2LedGreenOn(); break;
    case SENSOR2_LED_BLUE_PIN : Regulator_->sensor2LedBlueOn (); break;
    case MAIN_LED_RED_PIN     : Regulator_->mainLedRedOn     (); break;
    case MAIN_LED_GREEN_PIN   : Regulator_->mainLedGreenOn   (); break;
    case MAIN_LED_BLUE_PIN    : Regulator_->mainLedBlueOn    (); break;
    case RELAY_OUT_PIN         :
        PumpingStation_->switchOn    (); break;
    default:
        assert(false);
        break;
    }
}

void CSimulator::HW_ResetPortBit (int bit)
{
    using namespace MCU;
    switch (bit) {
    case SENSOR1_LED_RED_PIN  : Regulator_->sensor1LedRedOff  (); break;
    case SENSOR1_LED_GREEN_PIN: Regulator_->sensor1LedGreenOff(); break;
    case SENSOR1_LED_BLUE_PIN : Regulator_->sensor1LedBlueOff (); break;
    case SENSOR2_LED_RED_PIN  : Regulator_->sensor2LedRedOff  (); break;
    case SENSOR2_LED_GREEN_PIN: Regulator_->sensor2LedGreenOff(); break;
    case SENSOR2_LED_BLUE_PIN : Regulator_->sensor2LedBlueOff (); break;
    case MAIN_LED_RED_PIN     : Regulator_->mainLedRedOff     (); break;
    case MAIN_LED_GREEN_PIN   : Regulator_->mainLedGreenOff   (); break;
    case MAIN_LED_BLUE_PIN    : Regulator_->mainLedBlueOff    (); break;
    case RELAY_OUT_PIN        : PumpingStation_->switchOff    (); break;
    default:
        assert(false);
        break;
    }
}

bool CSimulator::HW_GetPortBit   (int bit)
{
    using namespace MCU;
    switch (bit) {
    case BUTTON1_PIN: return !Regulator_->getButton1State();
    case BUTTON2_PIN: return !Regulator_->getButton2State();
    case SENSOR1_PIN: return PumpingStation_->getSensorState(0);
    case SENSOR2_PIN: return PumpingStation_->getSensorState(1);
    default:
        assert(false);
        break;
    }
    return false;
}

}//namespace GUI
