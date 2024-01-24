#ifndef HW_SPECIFIC_H
#define HW_SPECIFIC_H
#include <cstdint>

namespace MCU{

using TPort  = void*;
using TBit   = int;

struct TPortBit{
    TPort Port;
    TBit  Bit;
};

enum EPortState{
    Off=0,
    On,
};

extern EPortState operator!(EPortState oth);

constexpr TPort  NoPort = nullptr;


}//namespace MCU

/*! \todo Все функции объявленный в этом файле надо прописать в соответствующем
 * cpp для используемого контроллера и платы.
 */
//============================================================================//
//                        Общая настройка оборудования                        //
//============================================================================//
/*!
 * \brief Инициализация микроконтроллера....
 */
extern void HW_MkInit();

//============================================================================//
//                              Цифровые порты                                //
//============================================================================//

extern void HW_PinToPortdata(int pin,MCU::TPort& dst_port,MCU::TBit& dst_bit);

inline void HW_PinToPortdata(int pin,MCU::TPortBit& dst){
    return HW_PinToPortdata(pin,(dst.Port),(dst.Bit));
}

inline MCU::TPortBit HW_PinToPortdata(int pin){
    MCU::TPortBit ret;
    HW_PinToPortdata(pin,ret);
    return ret;
}


/*!
 * \brief Настроить соответствующий вывод порта
 * \param bit    вывод порта
 * \param config конфигурация вывода порта
 * \return \b true если всё прошло успешно, иначе \b false
 */
extern bool HW_InitPortBit (MCU::TPort port, MCU::TBit bit,int config,int pull);

/*!
 * \brief Установить выход порта в 1
 * \param bit    вывод порта
 */
extern void HW_SetPortBit  (MCU::TPort port, MCU::TBit bit);

/*!
 * \brief Установить выход порта в 0
 * \param bit    вывод порта
 */
extern void HW_ResetPortBit(MCU::TPort port, MCU::TBit bit);

/*!
 * \brief Прочитать состояние порта
 * \param bit    вывод порта
 */
extern MCU::EPortState HW_GetPortBit(MCU::TPort port, MCU::TBit bit);

// ------------------------- Конфигурация портов ---------------------------- //
constexpr int PORT_INPUT     = 0; // Вход
constexpr int PORT_OUTPUT_PP = 1; // Выход (и верхний и нижний транзисторы включены)
constexpr int PORT_OUTPUT_OD = 2; // Выход (открытый коллектор)
constexpr int PORT_ALTERNATE = 3; // 10: Alternate function mode
constexpr int PORT_ANALOG    = 4; // 11: Analog mode


constexpr int PORT_NO_PULL    = 0b00; // Подтяжка вверх
constexpr int PORT_PULL_UP    = 0b01; // Подтяжка вверх
constexpr int PORT_PULL_DOWN  = 0b10; // Подтяжка вноз

//============================================================================//
//                                Системый таймер                             //
//============================================================================//
/*!
 * \brief Счетчик стабатывания прерывания по таймера. Процедура прерывания
 * должна инкременировать эту переменную
 */
extern int HW_MainSystemTimerFlag;

/*!
 * \brief Инициализация основного системного таймера
 * \return \b true если всё прошло успешно, иначе \b false
 */
extern bool HW_TimerInit ();

//============================================================================//
//                                     EEPROM                                 //
//============================================================================//
extern void HW_EepromInit  ();
extern void HW_EepromClear ();
extern void HW_EepromReservAddress(std::uint16_t address);
extern bool HW_EepromRead (std::uint16_t address,std::uint16_t& data);
extern bool HW_EepromWrite(std::uint16_t address,std::uint16_t  data);

#endif //HW_SPECIFIC_H
