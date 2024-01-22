#ifndef DIGIT_PORT_H
#define DIGIT_PORT_H
#include "hw_specific.h"

namespace MCU{


class CDigitPort
{
public:

    CDigitPort(TPortBit m_PortBit):CDigitPort(m_PortBit.Port,m_PortBit.Bit){}
    CDigitPort(TPort port,TBit bit):m_Port(port),m_PortBit{bit}{}
    bool         init  (int mode,int pull) {return HW_InitPortBit(m_Port,m_PortBit,mode,pull);}
    void         set   ()         {HW_SetPortBit  (m_Port,m_PortBit);}
    void         reset ()         {HW_ResetPortBit(m_Port,m_PortBit);}
    EPortState   get   ()         {return HW_GetPortBit(m_Port,m_PortBit);}
    void         set   (EPortState value){
      if (value==On) HW_SetPortBit  (m_Port,m_PortBit);
      else           HW_ResetPortBit(m_Port,m_PortBit);
      }

private:
    TPort m_Port;
    TBit  m_PortBit;
};

struct TPortSequenceItem{
    EPortState Value;      //!< Значение сигнала
    uint16_t   Duration;   //!< Длительность сигнала в 1 мСек (0 - конец последовательности)
};

/*!
 * \brief Класс, реализующий отображение последовательностей на порту
 */
class CPortSequence
{
public:
    CPortSequence(CDigitPort& port):m_Port(port),
        m_Sequence(nullptr),m_countSequence(0),m_SequenceFinished(false){};

    bool init(){return m_Port.init(PORT_OUTPUT_PP,PORT_NO_PULL);off();}
    /*!
     * \brief Необходимо вызывать с частотой 1 кГц
     */
    void tick();

    void setSequence   (const TPortSequenceItem* sequence){m_Sequence = sequence;/*m_countSequence=0;*/}
    void updateSequence(const TPortSequenceItem* sequence){m_Sequence = sequence;}
    bool isSequenceFinished(){return m_SequenceFinished;}

    void off(){m_Port.reset(); m_Sequence = secOff; m_countSequence=0; m_SequenceFinished=false;}
    void on() {m_Port.set()  ; m_Sequence = secOff; m_countSequence=0; m_SequenceFinished=false;}
    void set(EPortState val) {m_Port.set(val)  ; m_Sequence = secOff; m_countSequence=0; m_SequenceFinished=false;}
protected:
    CDigitPort&        m_Port;
    const TPortSequenceItem* m_Sequence;
    uint16_t           m_countSequence;
    bool               m_SequenceFinished;
private:
    const TPortSequenceItem* secOff = nullptr;
    /*!
     * \brief Обрабатываем заданную последовательность.
     *
     * \return Возвращает текущую интенсивность свечения светодиода этой
     * последовательности
     */
    EPortState processSequence(const TPortSequenceItem *);
};

}//namespace MCU


#endif // DIGIT_PORT_H
