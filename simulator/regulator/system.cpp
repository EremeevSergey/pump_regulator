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
const TPortSequenceItem seqMainB[]={off,off,off,off,off,off,off,off,END};

const TPortSequenceItem seqLedR[]={ON ,off,off,ON ,off,off,ON ,off,END};
const TPortSequenceItem seqLedG[]={off,ON ,off,off,ON ,off,off,ON ,END};
const TPortSequenceItem seqLedB[]={off,off,ON ,off,off,ON ,off,off,END};

}//namespace Start
}//namespace RGB_LED


CSystem::CSystem()
    : m_State   (EStateReset)
    , m_SysTimer()
    , m_Sensor1Led{SENSOR1_LED_RED_PIN,SENSOR1_LED_GREEN_PIN,SENSOR1_LED_BLUE_PIN}
    , m_Sensor2Led{SENSOR2_LED_RED_PIN,SENSOR2_LED_GREEN_PIN,SENSOR2_LED_BLUE_PIN}
    , m_MainLed   {MAIN_LED_RED_PIN   ,MAIN_LED_GREEN_PIN   ,MAIN_LED_BLUE_PIN}
    , m_SensorHi  {SENSOR2_PIN}
    , m_SensorLow {SENSOR1_PIN}
    , m_Relay     {RELAY_OUT_PIN}
    , m_TimerRgb     (CTimer::WrongDivider)
    , m_TimerMainLoop(CTimer::WrongDivider)
    , m_TimerSensors (CTimer::WrongDivider)
    , m_TimerButtons (CTimer::WrongDivider)
    , m_TimerRelay (CTimer::WrongDivider)
    , m_PumpWorking{false}
{}

void CSystem::init()
{
    HW_MkInit();

    m_SysTimer  .init();
    m_Sensor1Led.init();
    m_Sensor2Led.init();
    m_MainLed   .init();
    m_Relay.init();
    m_Relay.setHysteresisTime(RELAY_HYSTERESIS);
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
        m_Sensor1Led.tick();
        m_Sensor2Led.tick();
        m_MainLed   .tick();
    }
    if (m_SysTimer.getEvents(m_TimerSensors,true)>0){
        m_SensorHi.tick();
        m_SensorLow.tick();
    }

    if (m_SysTimer.getEvents(m_TimerRelay,true)>0) m_Relay.tick();

    switch (m_State) {
    case EStateReset:
        onReset();
        break;
    case EStartRGBBlink:
        onStartRGBBlink();
        break;
    case EMainLoop:
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

    m_Sensor1Led.setSequence(seqLedR ,seqLedG ,seqLedB);
    m_Sensor2Led.setSequence(seqLedR ,seqLedG ,seqLedB);
    m_MainLed   .setSequence(seqMainR,seqMainG,seqMainB);
}

void CSystem::onStartRGBBlink()
{
    if (m_MainLed.isRSequenceFinished()){
        m_Sensor1Led.set(Off,On,Off); //r=0,g=1,b=0 -> green
        m_Sensor2Led.set(Off,On,Off); //r=0,g=1,b=0 -> green
        m_MainLed.   set(Off,On,Off); //r=0,g=1,b=0 -> green
        m_State = EMainLoop;
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
    bool hi  = m_SensorHi.getState();
    bool Low = m_SensorLow.getState();
    if (hi) m_Sensor1Led.set(On,Off,Off);
    else {
        if (m_SensorHi.getType()==CSensor::NormalOpen) m_Sensor1Led.set(Off,On,Off);
        else                                           m_Sensor1Led.set(Off,Off,On);
    }

    if (Low) m_Sensor2Led.set(On,Off,Off);
    else {
        if (m_SensorLow.getType()==CSensor::NormalOpen) m_Sensor2Led.set(Off,On,Off);
        else                                            m_Sensor2Led.set(Off,Off,On);
    }


    if (Low && (hi || m_PumpWorking)){
        m_PumpWorking=true;
        m_Relay.switchOn();
        m_MainLed.set(On,Off,Off);
    }

    if (!Low){
        m_PumpWorking=false;
        m_Relay.switchOff();
        m_MainLed.set(Off,On,Off);
    }
}

}// namespace MCU
