#include "timer.h"
#include "config.h"

namespace MCU{

CTimer::CTimer()
    : m_MainEvents(0)
    , m_DividerCount(0)

    , m_SinleShortMsekCount(0)
    , m_SinleShortMsekFlag(ESingleTimerInactive)

    , m_SinleShortSekCount(0)
    , m_SinleShortSekFlag(ESingleTimerInactive)
    , m_SinleShortSekCount_msek(0)

{

}

void  CTimer::tick()
{
    if (HW_MainSystemTimerFlag>0){
        // Системый таймер сработал.
        // Для основной частоты
        m_MainEvents+=HW_MainSystemTimerFlag;
        HW_MainSystemTimerFlag=0;
        // Для остальных делителей
        for (int i=0;i<m_DividerCount;i++){
            m_Dividers[i].Counter++;
            if (m_Dividers[i].Counter>=m_Dividers[i].Value){
                m_Dividers[i].Counter=0;
                m_Dividers[i].Events++;
            }
        }
        // Для одиночных таймеров
        if (m_SinleShortMsekFlag==ESingleTimerCounting && m_SinleShortMsekCount>0){
            m_SinleShortMsekCount--;
            if (m_SinleShortMsekCount==0) m_SinleShortMsekFlag = ESingleTimerFinshed;
        }
        if (m_SinleShortSekFlag==ESingleTimerCounting && m_SinleShortSekCount>0){
            if (m_SinleShortSekCount_msek>0)
                m_SinleShortSekCount_msek--;
            if (m_SinleShortSekCount_msek==0) {
                uint32_t val = 1000000/MAIN_TIMER;
                m_SinleShortSekCount_msek = static_cast<uint16_t>(val);
                m_SinleShortSekCount--;
                if (m_SinleShortSekCount==0) m_SinleShortSekFlag = ESingleTimerFinshed;
            }
        }
    }
}

CTimer::Id_t CTimer::addDivider(int divider)
{
    if (divider<1 ||
            m_DividerCount>=MaxPrescalerCount) return WrongDivider;
    m_Dividers[m_DividerCount].Id      = m_DividerCount+1;
    m_Dividers[m_DividerCount].Value   = divider;
    m_Dividers[m_DividerCount].Counter = 0;
    m_Dividers[m_DividerCount].Events  = 0;
    m_DividerCount++;
    return m_DividerCount;
}

int CTimer::getEvents(Id_t divider_id, bool clear)
{
    int ret=0;
    if (divider_id==WrongDivider) return 0;
    if (divider_id==MainTimer){
        ret = m_MainEvents;
        if (m_MainEvents>0 && clear) m_MainEvents=0;
    }
    else if (divider_id<=m_DividerCount){
        divider_id--;
        ret = m_Dividers[divider_id].Events;
        if (m_Dividers[divider_id].Events>0 && clear)
            m_Dividers[divider_id].Events=0;
    }
    return ret;
}


void CTimer::single_mSek(uint16_t msek)
{
    m_SinleShortMsekFlag = ESingleTimerCounting;
    uint32_t val = 1000/MAIN_TIMER;
    m_SinleShortMsekCount = msek*static_cast<uint16_t>(val);
}

void CTimer::single_Sek(uint16_t sek)
{
    m_SinleShortSekFlag = ESingleTimerCounting;
    m_SinleShortSekCount = sek;
    uint32_t val = 1000000/MAIN_TIMER;
    m_SinleShortSekCount_msek = static_cast<uint16_t>(val);
}

}// namespace MCU

