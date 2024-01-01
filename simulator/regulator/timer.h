#ifndef TIMER_H
#define TIMER_H
#include "hw_specific.h"
#include "config.h"

namespace MCU{

/*!
 * \brief Класс основного таймера
 *
 * Базовая (основная) частота срабатывания таймера задается фиксированно,
 * настройкой "железного" таймера (в методе \ref init()).
 * Можно добавить до 16 (\ref MaxPrescalerCount) коэффициентов для деления основной частоты.
 *
 * Также можно активировать по одному "одиночному таймеру" с мСек и Сек выдержками.
 * Эти таймеры будут отсчитывать одночный интервал.
 */
class CTimer
{
public:
    typedef unsigned char Id_t;
    static const Id_t MainTimer=0;         //!< ID основного таймера
    static const Id_t WrongDivider=255;  //!< Не верный ID (для информирования об ошибке)
private:
    struct TPrescalerData{
        Id_t Id;       //!< Идентификатор делителя
        int  Value;    //!< Заданное значение делителя
        int  Counter;  //!< Текущий счетчик делителя
        int  Events;   //!< Количество срабатываний делителя
    };

    static const Id_t     MaxPrescalerCount = 10; //!< Количество доступных делителей (на стадии компиляции)

public:
    /*!
     * \brief Конструктор. Настраивает внутренние переменные
     */
    CTimer();

    /*!
     * \brief Инициализация железа. Настройка базовой чатоты срабатывания
     * таймера.
     * \return \b true - в случае удачной настойки оборудования, иначе \b false
     */
    bool init(){return HW_TimerInit();}

    /*!
     * \brief Основной цикл таймера. Должен вызываться постоянно
     */
    void  tick();

    /*!
     * \brief Добавить дополнительный делитель (основной частоты) таймера
     * \param divider - значение делителя (1...n)
     * \return идентификатор добавленного делителя или  \ref WrongPrescaler в случае ошибки
     * (например, уже все делители заняты или значение делителя равно 0.
     */
    Id_t addDivider(int divider);

    /*!
     * \brief Возвращает количество срабатывая таймера для заданного делителя
     * \param divider_id ID делителя, для которого запрашиваем количество
     * срабатываний. (не забываем, что \ref MainTimer - это основной таймер)
     * \param clear \b true очистить количество срабатываний,
     * \b false (по умолчанию) - не очищать количство срабатываний
     * \return количество срабатываний или 0. Для не зарегистрированных ID возвращается тоже 0.
     */
    int  getEvents(Id_t divider_id, bool clear=false);

    /*!
     * \brief Запустить отсчет миллисекундного интервала.
     * Если ранее отсчет уже был запущен, то он сбрасывается и запускается заново.
     * Никакие флаги при этом не устанавливаются.
     * \param msek Длина интервала в мСек
     */
    void single_mSek(uint16_t msek);

    /*!
     * \brief Проверить закончился или нет отсчет миллисекундного интервала.
     * \return \b true если закончился иначе \b false.
     */
    bool isSingle_mSek(){if (m_SinleShortMsekFlag==ESingleTimerFinshed) return true; else return false;}

    /*!
     * \brief Запустить отсчет секундного интервала
     * Если ранее отсчет уже был запущен, то он сбрасывается и запускается заново.
     * Никакие флаги при этом не устанавливаются.
     * \param sek Длина интервала в Сек
     */
    void single_Sek(uint16_t sek);

    /*!
     * \brief Проверить закончился или нет отсчет секундного интервала.
     * \return \b true если закончился иначе \b false.
     */
    bool isSingle_Sek(){if (m_SinleShortSekFlag==ESingleTimerFinshed) return true; else return false;}
private:
    int            m_MainEvents;                  //!< Количество не обработанных срабатываний основной частоты
    Id_t           m_DividerCount;                //!< Количество дополнительных делителей основной частоты
    TPrescalerData m_Dividers[MaxPrescalerCount]; //!< Данные для дополнительных делителей основной частоты

    enum ESingleimerState{
        ESingleTimerInactive=0,
        ESingleTimerCounting,
        ESingleTimerFinshed
    };

    uint16_t         m_SinleShortMsekCount;  //!< Счетчик миллисекундого интервала для одиночного таймера
    ESingleimerState m_SinleShortMsekFlag;   //!< Состояние миллисекундого  одиночного таймера

    uint16_t         m_SinleShortSekCount;      //!< Счетчик секундого интервала для одиночного таймера
    ESingleimerState m_SinleShortSekFlag;       //!< Состояние секундого одиночного таймера
    uint16_t         m_SinleShortSekCount_msek; //!< Делитель основной частоты до 1 Гц, для работы секундого одиночного таймера
};

}// namespace MCU

#endif // TIMER_H
