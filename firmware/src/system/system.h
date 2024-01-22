#ifndef SYSTEM_H
#define SYSTEM_H

#include "rgb_led.h"
#include "timer.h"
#include "sensor.h"
#include "relay.h"
#include "button.h"
#include "settings.h"

namespace MCU {

/*!
 * \brief Основной класс приложения.
 *
 * Включает всебя всю логику роботы устройста и все компоненты устройства:
 * светодиоды, пищалки, радиомодули....
 */
class CSystem
{
public:
    CSystem();

    /*!
     * \brief Инициализация "железа" устройства (настройки портов, таймеров ....)
     */
    void init();

    /*!
     * \brief Основной цикл работы устройства
     */
    void tick();

private:
    enum EState{
        EStateReset = 0,
        EStartRGBBlink,
        EMainLoopAuto,
        EMainLoopManual,
    };

    /*!
     * \brief Вызывается после сброса устройства.
     */
    void       onReset();


    void       onStartRGBBlink();
    /*!
     * \brief Основной цикл работы устройства
     */
    void       mainLoop      ();
    void       mainLoopAuto  (bool hi,bool low);
    void       mainLoopManual();
    void       switchToAuto  ();
    void       switchToManual();
private:
    EState         m_State;
    CTimer         m_SysTimer;
    CRgbLed        m_SensorLowLed;
    CRgbLed        m_SensorHiLed;
    CRgbLed        m_MainLed;
    CSensor        m_SensorLow;
    CSensor        m_SensorHi;
    CRelay         m_Relay;
    CButton        m_ButtonLow;
    CButton        m_ButtonHi;
    CSettings      m_Settings;
private:
    CTimer::Id_t  m_TimerRgb;
    CTimer::Id_t  m_TimerMainLoop;
    CTimer::Id_t  m_TimerSensors;
    CTimer::Id_t  m_TimerButtons;
    CTimer::Id_t  m_TimerRelay;

    bool          m_PumpWorking;

public:
};

extern CSystem System;

}// namespace MCU

#endif // SYSTEM_H
