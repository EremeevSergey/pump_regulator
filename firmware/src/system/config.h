#ifndef CONFIG_H
#define CONFIG_H

namespace MCU{

//--------------------------------------------------------------------------//
//                       Порты (ножки) микроконтроллера                     //
//--------------------------------------------------------------------------//
// Ножки RGB-светодиодов
constexpr int SENSOR1_LED_RED_PIN   = 6;
constexpr int SENSOR1_LED_GREEN_PIN = 3;
constexpr int SENSOR1_LED_BLUE_PIN  = 17;

constexpr int SENSOR2_LED_RED_PIN   = 9;
constexpr int SENSOR2_LED_GREEN_PIN = 8;
constexpr int SENSOR2_LED_BLUE_PIN  = 7;

constexpr int MAIN_LED_RED_PIN    = 14;
constexpr int MAIN_LED_GREEN_PIN  = 13;
constexpr int MAIN_LED_BLUE_PIN   = 12;

//--------------------------------------------------------------------------//
//                                 Выход на реле                            //
//--------------------------------------------------------------------------//
constexpr int RELAY_OUT_PIN    = 11;
constexpr int RELAY_HYSTERESIS = 1000; // 1 секунда
//--------------------------------------------------------------------------//
//                               Вход герконов                              //
//--------------------------------------------------------------------------//
constexpr int SENSOR1_PIN = 18;
constexpr int SENSOR2_PIN = 19;

//--------------------------------------------------------------------------//
//                                   Кнопки                                 //
//--------------------------------------------------------------------------//
constexpr int BUTTON1_PIN = 2;
constexpr int BUTTON2_PIN = 10;

//--------------------------------------------------------------------------//
//         Тайминги, для обработки аппаратных устройств (мкСек)             //
//--------------------------------------------------------------------------//
constexpr int MAIN_TIMER = 1000; // Период (мкСек) основного таймера устройства

// Все остальные тийминги будут опираться на MAIN_TIMER (делиться на него)
constexpr int RGB_LED_TIMER = 1000; // Период обработки RGB светодиода (частота 1кГц)
static_assert(RGB_LED_TIMER % MAIN_TIMER==0,"Должно кратно делиться на MAIN_TIMER");

constexpr int MAIN_LOOP_PERIOD = 10000; // Переодичность основного цикла. Думаю 100 раз в секунду - достаточно.
static_assert(MAIN_LOOP_PERIOD % MAIN_TIMER==0,"Должно кратно делиться на MAIN_TIMER");

constexpr int SENSORS_PERIOD = 10000; // Переодичность опроса сенсоров. Думаю 100 раз в секунду - достаточно.
static_assert(MAIN_LOOP_PERIOD % MAIN_TIMER==0,"Должно кратно делиться на MAIN_TIMER");

constexpr int BUTTONS_PERIOD = 10000; // Переодичность опроса кнопок. Думаю 100 раз в секунду - достаточно.
static_assert(MAIN_LOOP_PERIOD % MAIN_TIMER==0,"Должно кратно делиться на MAIN_TIMER");

constexpr int RELAY_PERIOD = 1000; // Переодичность опроса реле. 1мсек, чтобы гистерезис отрабатывать.
static_assert(MAIN_LOOP_PERIOD % MAIN_TIMER==0,"Должно кратно делиться на MAIN_TIMER");

}//namespace MCU

#endif // CONFIG_H
