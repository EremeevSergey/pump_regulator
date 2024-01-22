#include "digit_port.h"

namespace MCU{

EPortState operator!(EPortState oth){
    if (oth ==On) return Off;
    else return On;
}

void CPortSequence::tick()
{
    if (m_Sequence!=nullptr){
        m_countSequence++;
        if (m_countSequence>=50000) m_countSequence=0; // Рассчитываем, что максимальная
        // длина последовательности не будет больше 50000*1мСек = 50 сек.
        // Надо очень извратиться, чтобы сформировать такую последовательность

        // Тут выполняем последовательности
        m_Port.set(processSequence(m_Sequence));
    }
}

EPortState CPortSequence::processSequence(const TPortSequenceItem *sec)
{
    if (sec){
        // Получаем сначала полную длительность последовательности
        uint16_t duration=0;
        for (int i=0;sec[i].Duration!=0/* || sec[i].Value!=0*/;i++){
            duration+=sec[i].Duration;
        }
        if (duration>0){ // Общая длина последовательноть больше нуля - обрабатываем её
            if (m_countSequence>duration)
                m_SequenceFinished=true;
            duration = m_countSequence % duration; // Получаем текущее положение в
            // данной последовательности в зависимости от countSequence  - тупо
            // остаток от деления счетчика на полную длинну последовательности

            // А теперь ищем элемент последовательности в
            for (int i=0;sec[i].Duration!=0/* || sec[i].Value!=0*/;i++){
                if (duration<sec[i].Duration) return sec[i].Value;
                duration-=sec[i].Duration;
            }
        }
    }
    return Off; // Нет последовательности
}

}//namespace MCU
