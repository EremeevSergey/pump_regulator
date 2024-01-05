#include "hw_specific.h"
#include "system.h"
#include "config.h"

namespace MCU {

CSystem System;

/*!
  Последовательности для моргания светодиодами
*/
namespace RGB_LED {
constexpr TPortSequenceItem END{EPortState::On,0};
namespace Start {
// Стартовая "светомузыка" по 500 мсек
// main  r g - r g - r g
// led2  r g b r g b r g
// led1  r g b r g b r g
constexpr int ST=500;// длина шага в мсек
constexpr TPortSequenceItem ON {EPortState::On ,ST};
constexpr TPortSequenceItem off{EPortState::Off,ST};

const TPortSequenceItem seqMainR[]={ON ,off,off,ON ,off,off,ON ,off,END};
const TPortSequenceItem seqMainG[]={off,ON ,off,off,ON ,off,off,ON ,END};
const TPortSequenceItem seqMainB[]={off,off,ON ,off,off,ON ,off,off,END};

const TPortSequenceItem seqLedR[]={ON ,off,off,ON ,off,off,ON ,off,END};
const TPortSequenceItem seqLedG[]={off,ON ,off,off,ON ,off,off,ON ,END};
const TPortSequenceItem seqLedB[]={off,off,ON ,off,off,ON ,off,off,END};

}//namespace Start
}//namespace RGB_LED


CSystem::CSystem()
    : m_State   (EStateReset)
    , m_SysTimer()
    , m_SensorLowLed{SENSOR1_LED_RED_PIN,SENSOR1_LED_GREEN_PIN,SENSOR1_LED_BLUE_PIN}
    , m_SensorHiLed {SENSOR2_LED_RED_PIN,SENSOR2_LED_GREEN_PIN,SENSOR2_LED_BLUE_PIN}
    , m_MainLed     {MAIN_LED_RED_PIN   ,MAIN_LED_GREEN_PIN   ,MAIN_LED_BLUE_PIN}
    , m_SensorLow {HW_PinToPortdata(SENSOR1_PIN)}
    , m_SensorHi  {HW_PinToPortdata(SENSOR2_PIN)}
    , m_Relay     {HW_PinToPortdata(RELAY_OUT_PIN)}
    , m_ButtonLow {HW_PinToPortdata(BUTTON1_PIN)}
    , m_ButtonHi  {HW_PinToPortdata(BUTTON2_PIN)}
    , m_TimerRgb     (CTimer::WrongDivider)
    , m_TimerMainLoop(CTimer::WrongDivider)
    , m_TimerSensors (CTimer::WrongDivider)
    , m_TimerButtons (CTimer::WrongDivider)
    , m_TimerRelay   (CTimer::WrongDivider)
    , m_PumpWorking{false}
{}

void CSystem::init()
{
    HW_MkInit();

    m_SensorLow.init();
    m_SensorHi .init();

    m_SysTimer  .init();
    m_SensorHiLed.init();
    m_SensorLowLed.init();
    m_MainLed   .init();
    m_Relay.init();
    m_Relay.setHysteresisTime(RELAY_HYSTERESIS);
    m_ButtonLow.init();
    m_ButtonHi .init();
    m_PumpWorking = false;
    // Программируем таймеры для опроса устройств
    m_TimerRgb      = m_SysTimer.addDivider(RGB_LED_TIMER   /MAIN_TIMER);
    m_TimerMainLoop = m_SysTimer.addDivider(MAIN_LOOP_PERIOD/MAIN_TIMER);
    m_TimerSensors  = m_SysTimer.addDivider(SENSORS_PERIOD  /MAIN_TIMER);
    m_TimerButtons  = m_SysTimer.addDivider(BUTTONS_PERIOD  /MAIN_TIMER);
    m_TimerRelay    = m_SysTimer.addDivider(RELAY_PERIOD    /MAIN_TIMER);

    m_State = EStateReset;
}

void CSystem::tick()
{
    m_SysTimer.tick();
    if (m_SysTimer.getEvents(m_TimerRgb,true)>0) {
        m_SensorHiLed.tick();
        m_SensorLowLed.tick();
        m_MainLed   .tick();
    }
    if (m_SysTimer.getEvents(m_TimerSensors,true)>0){
        m_SensorHi.tick();
        m_SensorLow.tick();
    }

    if (m_SysTimer.getEvents(m_TimerRelay,true)>0) m_Relay.tick();

    if (m_SysTimer.getEvents(m_TimerButtons,true)>0){
        m_ButtonLow.tick();
        m_ButtonHi.tick();
    }

    switch (m_State) {
    case EStateReset:
        onReset();
        break;
    case EStartRGBBlink:
        onStartRGBBlink();
        break;
    case EMainLoopAuto:
    case EMainLoopManual:
        if (m_SysTimer.getEvents(m_TimerMainLoop,true)>0){
            mainLoop();
        }
        break;
    }
}

void CSystem::onReset()
{
    using namespace RGB_LED::Start;
    m_State = EStartRGBBlink;

    m_SensorHiLed.setSequence (seqLedR ,seqLedG ,seqLedB);
    m_SensorLowLed.setSequence(seqLedR ,seqLedG ,seqLedB);
    m_MainLed   .setSequence  (seqMainR,seqMainG,seqMainB);
}

void CSystem::onStartRGBBlink()
{
    using namespace RGB_LED;
    if (m_MainLed.isRSequenceFinished()){
        m_SensorHiLed. set(CRgbLed::Green);
        m_SensorLowLed.set(CRgbLed::Green);
        m_MainLed.     set(CRgbLed::Green);
        m_State = EMainLoopAuto;
    }
}


//--------------------------------------------------------------------------//
//                       Основной цикл работы устройства.                   //
//                     До этого было только приготовление.                  //
//                                                                          //
//      Естествено, чтобы все железки у нас работали, как-бы в "фоне",      //
//   мы ДОЛЖНЫ проходить эту функцию, как можно быстрее и по возможности    //
//   не использовать длинных циклов или программных никаких задержек.       //
//--------------------------------------------------------------------------//

void CSystem::mainLoop()
{
    bool Hi  = m_SensorHi.getState();
    bool Low = m_SensorLow.getState();
    if (Hi) m_SensorHiLed.set(CRgbLed::Red);
    else {
        if (m_SensorHi.getType()==CSensor::NormalOpen)
              m_SensorHiLed.set(CRgbLed::Green);
        else  m_SensorHiLed.set(CRgbLed::Blue);
    }

    if (Low) m_SensorLowLed.set(CRgbLed::Red);
    else {
        if (m_SensorLow.getType()==CSensor::NormalOpen)
              m_SensorLowLed.set(CRgbLed::Green);
        else  m_SensorLowLed.set(CRgbLed::Blue);
    }

    // Проверяем переключение датчика Номально открытый/закрытый
    if (m_ButtonLow.isLongPress() && m_ButtonHi.getState()!=CButton::Press){
        m_SensorLow.invertType();
        m_ButtonLow.resetCounter();
        // Тут можно анимации добавить
    }

    // Проверяем переключение датчика Номально открытый/закрытый
    if (m_ButtonHi.isLongPress() && m_ButtonLow.getState()!=CButton::Press){
        m_SensorHi.invertType();
        m_ButtonHi.resetCounter();
        // Тут можно анимации добавить
    }

    // Проверяем переключение режима Авто/Ручной
    if (m_ButtonLow.isLongPress() && m_ButtonHi.isLongPress()){
        if      (m_State==EMainLoopAuto)   switchToManual();
        else if (m_State==EMainLoopManual) switchToAuto();
        m_ButtonLow.resetCounter();
        m_ButtonHi .resetCounter();
        // Тут можно анимации добавить
    }

    if (m_State==EMainLoopManual) mainLoopManual();
    else                          mainLoopAuto  (Hi,Low);
}

void CSystem::switchToAuto  ()
{
    m_State = EMainLoopAuto;
    m_PumpWorking=false;
    m_Relay.switchOff();
    m_MainLed.set(CRgbLed::Green);
}

void CSystem::switchToManual()
{
    m_State = EMainLoopManual;
    m_PumpWorking=false;
    m_Relay.switchOff();
    m_MainLed.set(CRgbLed::Blue);
}

void CSystem::mainLoopAuto  (bool Hi,bool Low)
{
    if (Low && (Hi || m_PumpWorking)){
        m_PumpWorking=true;
        m_Relay.switchOn();
        m_MainLed.set(CRgbLed::Red);
    }

    if (!Low){
        m_PumpWorking=false;
        m_Relay.switchOff();
        m_MainLed.set(CRgbLed::Green);
    }
}

void CSystem::mainLoopManual()
{
    auto& ButtonOn  = m_ButtonLow;
    auto& ButtonOff = m_ButtonHi;
    if (ButtonOn.isClick()){
        m_PumpWorking=true;
        m_Relay.switchOn();
        m_MainLed.set(CRgbLed::Red);
    }
    else if (ButtonOff.isClick()){
        m_PumpWorking=false;
        m_Relay.switchOff();
        m_MainLed.set(CRgbLed::Blue);
    }
}

}// namespace MCU
