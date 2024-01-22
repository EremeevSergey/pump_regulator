#ifndef SETTINGS_H
#define SETTINGS_H
#include "sensor.h"

namespace MCU{

/*!
 * \brief Хранит настройки датчиков.
 *
 * Будем записывать все настройки в одном 16-и битном слове и три раза. Чтобы точно убедится,
 * что прочитали то, что хотели
 */
class CSettings
{
public:
    enum class EError{
        No=0,
        LoadSaveError,
    };
public:
    CSettings();
    /*!
     * \brief Инициализируем объект класса - подготавливаем железо
     */
    void init();

    /*!
     * \brief Перезагружаем из энергонезависимой памяти настройки.
     *
     * Если произошла какая-то ошибка, то приводим всё к к дефолтному состоянию.
     */
    bool load();

    /*!
     * \brief Сохраняем в энергонезависимую памяти настройки.
     *
     * Если произошла какая-то ошибка, то приводим всё к к дефолтному состоянию.
     */
    bool save();

    void resetToDefault();

    EError getError(){return m_Error;}
public:
    CSensor::EType Sensor1;
    CSensor::EType Sensor2;
private:
    constexpr static std::uint16_t AddressCopy1=0x0000;
    constexpr static std::uint16_t AddressCopy2=0x0001;
    constexpr static std::uint16_t AddressCopy3=0x0003;

    constexpr static std::uint8_t NormalOpen  =0x55;
    constexpr static std::uint8_t NormalClosed=0xAA;

    std::uint16_t SensorStateToWord();
    bool          WordToSensorState(std::uint16_t word);

    EError        m_Error;
};

}// namespace MCU

#endif // SETTINGS_H
