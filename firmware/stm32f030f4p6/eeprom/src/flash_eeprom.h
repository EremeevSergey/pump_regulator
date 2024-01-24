#ifndef FLASH_EEPROM_H
#define FLASH_EEPROM_H
#include <cstdint>
#include <cassert>

/*!
 * \brief Внешняя функция, стирающая странице во внутренней флэш-памяти.
 * \details Её надо где-то в проекте определить (на базе STL, HAL, CMSIS...)
 * \param addr - адрес начала стираемой страницы
 * \return \b true  в случае удачи, иначе \b false
 */
extern bool FLASH_ErasePage(void* addr);

/*!
 * \brief Внешняя функция, записывающая во внутреннюю флэш-память полу-слово (16 бит)
 * \param address Адрес во флэш-памяти
 * \param data    аписываемое значение (16 бит)
 * \return \b true  в случае удачи, иначе \b false
 */
extern bool FLASH_ProgramHalfWord(void* address,std::uint16_t data);

/*!
 * \brief Класс реализующий эмуляцию EEPROM на встроенной флэш-памяти.
 * см. AN4061 \link https://www.st.com/en/embedded-software/stsw-stm32117.html
 * Используются 16-и битные слова для адреса и данных. Страниц 2 шт!!!.
 * \details Как этим пользоваться:
 *
 * Виртуальная EEPROM, согласно AN4061, хранит данные в виде пары
 * Значение, Миртуальный_Адрес. Т.е. Все виртуальные адреса должны быть известны заранее
 * чтобы можно было правильно их переносить с одной страницы на другую.
 * Поэтому делаем следующим образом:
 * \code{.cpp}
 * CFlashEeprom<ADDRESS_COUNT> EEPROM; //Создаём объект класса. Здесь ADDRESS_COUNT - общее
 *                                     //кол-во используемых виртуальных адесов
 * // Далее в обязательном порядке мы должны записать во внутренний массив класса все
 * // виртуальные адреса, которые мы в дальнейшем планируем использовать
 * EEPROM.AddVirtualAddrress(VirtAddress1); // Добавляем первый виртуальный адрес
 * EEPROM.AddVirtualAddrress(VirtAddress2); // Добавляем второй виртуальный адрес
 * ....
 * EEPROM.AddVirtualAddrress(VirtAddressN); // Добавляем последний виртуальный адрес
 *
 * // Естественно, если у нас есть массив со списком виртуальных адресов, то можно это сделть в
 * // цикле. Так-же, если у нас адреса идут по порядку (0x0000,0x0001,0x0002...), може сделать
 * // и так:
 * // for (unsigned i=0;i<ADDRESS_COUNT;++i) EEPROM.SetVirtualAddrress(i);
 *
 * EEPROM.Init(); // В начале программы \b ОБЯЗАТЕЛЬНО!!! вызываем метод, который настраивает
 *                // внутренние переменные, и приводит заголовки страниц в актцальное состояние
 *                // если это требуется
 * // Всё, далее в любой части программы може читать и записывать переменные:
 *
 * if (EEPROM.WriteVariable(VirtAddr1,Var1)!=true){
 *      // Тут можем обработать ошибку записи
 * }
 * ......
 * EEPROM.WriteVariable(VirtAddr4,Var4);
 * ......
 * if (EEPROM.ReadVariable(VirtAddr1,Var1)!=true){
 *      // Тут можем обработать ошибку чтения
 * }
 * \endcode
 *
 * Дополнительно к этому, надо \b ОБЯЗАТЕЛЬНО!!! где-то определить функции, которые работают с
 * реальным железом \ref FLASH_ErasePage и \ref FLASH_ProgramHalfWord;
 *
 */
template <int m_VarsCount>
class CFlashEeprom
{
    using TWord    = std::uint16_t;
    using TElement = std::uint32_t;
    using TAddr    = TWord;
    using TData    = TWord;

    constexpr static TElement ErasedElement = 0xFFFFFFFF;
    /*!
     * \brief Статусы страницы
     */
    constexpr static TWord ErasedPage      = 0xFFFF; //!< Пустая страница
    constexpr static TWord ReceiveDataPage = 0xEEEE; //!< В состоянии принятия данных из предыдущей страницы
    constexpr static TWord ValidPage       = 0x0000; //!< Активная в данный момент

    constexpr static TAddr WrongVirtAddr   = 0xFFFF; //!< Не действительный виртуальный адрес
public:

    CFlashEeprom(void* start_eeprom_flash_addr,int page_size_byte)
        : m_StartEepromAddress{static_cast<char*>(start_eeprom_flash_addr)}
        , m_PageSize{page_size_byte}
        , m_AddrPage0{0}
        , m_AddrPage1{0}
    {
        assert(m_PageSize%(2*sizeof(TWord))==0);
        m_AddrPage0 = reinterpret_cast<TWord*>(m_StartEepromAddress);
        m_AddrPage1 = reinterpret_cast<TWord*>(m_StartEepromAddress+m_PageSize);
        for (int i=0;i<m_VarsCount;++i) m_VarsAddr[i] = WrongVirtAddr;
    }
    CFlashEeprom(const CFlashEeprom& )=delete;
    CFlashEeprom(      CFlashEeprom&&)=delete;
    CFlashEeprom& operator=(const CFlashEeprom& )=delete;
    CFlashEeprom& operator=(      CFlashEeprom&&)=delete;

    /*!
     * \brief Записываем виртуальный адрес во внутреннее храилище, для того,
     * чтобы знать какие переменные и для каких виртуальных адресов сохранять
     * \param index
     * \param addr
     */
    void AddVirtualAddrress(TAddr addr){
        assert(addr!=WrongVirtAddr);
        for (int i=0;i<m_VarsCount;++i){
            if (m_VarsAddr[i] == WrongVirtAddr){
                m_VarsAddr[i] = addr;
                break;
            }
        }
    }

    bool Init();
    bool ReadVariable (TAddr VirtAddress, TData& Data);
    bool WriteVariable(TAddr VirtAddress, TData Data);

    /*!
     * \brief Обнуляет виртуальную EEPROM
     * \details Эта функция стирает ВСЕ страницы,
     * и устанавливает заголовок Страницы0 равным VALID_PAGE.
     * \return \b true или в случае ошибки \b false.
     */
    bool Format();
protected:
    /*!
     * \brief Операция
     */
    enum class TOperation{
        Read ,//!< Чтение
        Write,//!< Запись
    };

    constexpr static int PageIndex0       = 0; //!< Страница 0
    constexpr static int PageIndex1       = 1; //!< Страница 1
    constexpr static int InvalidPageIndex =-1; //!< Не правильный идекс страницы
    /*!
     * \brief Ищем активную страницу для запрашиваемой операции
     * \param operation Операция (запись или чтение)
     * \return Адрес активной страницы или \ref InvalidPageIndex
     */
    int  FindValidPage(TOperation operation);

    enum class TResult{
        Error ,//!< Ошибка
        Ok  ,//!< Всё хорошо
        PageFull,
    };

    TResult VerifyPageFullWriteVariable(TAddr VirtAddress, TData Data);

    bool PageTransfer  (TAddr VirtAddress, TData Data);

    /*!
     * \brief ErasePage Стираем страницу памяти.
     * \details Сначала проверяем, что страница действительно пустая (\ref TestPageErased),
     * и если это не так вызывается реальная функция стирания страницы.
     * Так, мы съэкономим немного ресурс флеша если устройство часто включается выключается.
     * При этом не значительно потеряем в скорости, и то, только в моменты переключения страниц
     * \param start_address
     * \return
     */
    bool ErasePage     (void* start_address){
        if (!TestPageErased(start_address))
            return FLASH_ErasePage(start_address);
        return true;
    }
private:
    char*  m_StartEepromAddress;//!< Начальный адрес во внутренней флэш-памяти для эмуляции EEPROM
    int    m_PageSize ; //!< Размер страницы
    TWord* m_AddrPage0;
    TWord* m_AddrPage1;
    TAddr  m_VarsAddr[m_VarsCount];
    /*
     * Структура страницы следущая (размер всех данных TWord):
     * ----начало страницы-----
     * header     XXXX
     * value1     virt_add1
     * value2     virt_add3
     * value3     virt_add3
     *   . . . . . . .
     * value_n    virt_add_n
     * ----конец страницы-----
     */

    /*!
     * \brief Возвращает начальный адрес страницы. На валидность номер страницы не проверяется
     * \param page номер страницы.
     */
    auto getPageStartAddress(int page){
        return reinterpret_cast<TData*>(m_StartEepromAddress + page*m_PageSize);}

    /*!
     * \brief Возвращает адрес, следующй за концом страницы.
     * На валидность номер страницы не проверяется
     * \param page номер страницы.
     */
    auto getPageEndAddress(int page){
        return reinterpret_cast<TData*>(m_StartEepromAddress + (page+1)*m_PageSize);}

    /*!
     * \brief Проверяем, что страница действительно пустая.
     * \param start_address Началбный адрес страницы
     * \return \b true в случае, если страница пустая иначе \b false
     */
    bool TestPageErased(void* start_address);
};


template <int m_VarsCount>
typename CFlashEeprom<m_VarsCount>::TResult
CFlashEeprom<m_VarsCount>::VerifyPageFullWriteVariable(TAddr VirtAddress, TData Data){
    auto valid_page_number = FindValidPage(TOperation::Write);
    if (valid_page_number==InvalidPageIndex) return TResult::Error;
    // Начало активной страницы
    auto address = getPageStartAddress(valid_page_number)+2; // пропускаем заголовок
    // Адрес, следующий сразу за концом страницы
    auto page_end_address = getPageEndAddress  (valid_page_number);
    /* Check each active page address starting from begining */
    while (address < page_end_address){
        /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
        if (*(reinterpret_cast<TElement*>(address)) == ErasedElement){
            /* Set variable data */
            /* If program operation was failed, a Flash error code is returned */
            if (FLASH_ProgramHalfWord(address, Data) != true) return TResult::Error;
            /* Set variable virtual address */
            if (FLASH_ProgramHalfWord(address + 1, VirtAddress) != true) return TResult::Error;
            return TResult::Ok;
        }
        else address = address + 2;// Next address location
    }
    /* Return PAGE_FULL in case the valid page is full */
    return TResult::PageFull  ;//PAGE_FULL;
}

template <int m_VarsCount>
bool CFlashEeprom<m_VarsCount>::Format(){
    if (ErasePage      (m_AddrPage0)           !=true) return false;
    if (FLASH_ProgramHalfWord(m_AddrPage0, ValidPage)!=true) return false;
    return  ErasePage(m_AddrPage1);
}

template <int m_VarsCount>
int CFlashEeprom<m_VarsCount>::FindValidPage(TOperation operation){
    TWord page0_status = *m_AddrPage0;
    TWord page1_status = *m_AddrPage1;
    switch (operation)  {
    case TOperation::Write:   // ---- Write operation ----
        if (page1_status == ValidPage) {                                  // Page0 receiving data
            if (page0_status == ReceiveDataPage) return PageIndex0;       // Page0 valid
            else                                 return PageIndex1;       // Page1 valid
        }
        else if (page0_status == ValidPage){                              // Page1 receiving data
            if (page1_status  == ReceiveDataPage)return PageIndex1;       // Page1 valid
            else                                 return PageIndex0;       // Page0 valid
        }
        else                                     return InvalidPageIndex; // No valid Page

    case TOperation::Read:  /* ---- Read operation ---- */
    default:
        if      (page0_status == ValidPage)      return PageIndex0;       // Page0 valid
        else if (page1_status == ValidPage)      return PageIndex1;       // Page1 valid
        else                                     return InvalidPageIndex; // No valid Page
    }
}

template <int m_VarsCount>
bool CFlashEeprom<m_VarsCount>::ReadVariable (TAddr VirtAddress, TData& Data)
{
    auto  valid_page_number = FindValidPage(TOperation::Read);
    if (valid_page_number==InvalidPageIndex) return false;
    // Начало активной страницы
    auto page_start_address = getPageStartAddress(valid_page_number)+2; // пропускаем заголовок
    // Адрес, следующий сразу за концом страницы
    auto address            = getPageEndAddress  (valid_page_number);

    /* Check each active page address starting from end */
    while (address > page_start_address){
        /* Get the current location content to be compared with virtual address */
        TAddr AddressValue = *(address-1);

        /* Compare the read address with the virtual address */
        if (AddressValue == VirtAddress){
            /* Get content of Address-2 which is variable value */
            Data = *(address - 2);
            return true;
        }
        else address -= 2;//  Next address location
    }
    return false;
}

template <int m_VarsCount>
bool CFlashEeprom<m_VarsCount>::WriteVariable(TAddr VirtAddress, TData Data)
{
    /* Write the variable virtual address and value in the EEPROM */
    auto Status = VerifyPageFullWriteVariable(VirtAddress, Data);
    if (Status==TResult::Ok) return true;
    /* In case the EEPROM active page is full */
    if (Status == TResult::PageFull) // Perform Page transfer
        return PageTransfer(VirtAddress, Data);
    return false;
}

template <int m_VarsCount>
bool CFlashEeprom<m_VarsCount>::PageTransfer (TAddr VirtAddress, TData Data)
{
    /* Get active Page for read operation */
    auto valid_page_number = FindValidPage(TOperation::Read);
    if (valid_page_number==InvalidPageIndex) return false;

    TWord *new_page_address{m_AddrPage1}, *old_page_address{m_AddrPage0};
    if (valid_page_number == PageIndex1){       /* Page1 valid */
        /* New page address where variable will be moved to */
        new_page_address = m_AddrPage0;
        /* Old page address where variable will be taken from */
        old_page_address = m_AddrPage1;
    }
    else if (ValidPage != PageIndex0) return false;       /* No valid Page */
    /* Page0 valid */

    /* Set the new Page status to RECEIVE_DATA status */
    if (FLASH_ProgramHalfWord(new_page_address, ReceiveDataPage) != true)  return false;

    /* Write the variable passed as parameter in the new active page */
    if (VerifyPageFullWriteVariable(VirtAddress, Data) != TResult::Ok)  return false;

    /* Transfer process: transfer variables from old to the new active page */
    for (int VarIdx = 0; VarIdx < m_VarsCount && m_VarsAddr[VarIdx]!=WrongVirtAddr; ++VarIdx)  {
        TData var_data{0};
        if (m_VarsAddr[VarIdx] != VirtAddress) { /* Check each variable except the one passed as parameter */
            /* Read the other last variable updates */
            /* In case variable corresponding to the virtual address was found */
            if (ReadVariable(m_VarsAddr[VarIdx], var_data) == true) {
                /* Transfer the variable to the new active page */
                if (VerifyPageFullWriteVariable(m_VarsAddr[VarIdx], var_data) != TResult::Ok) return false;
            }
        }
    }

    /* Erase the old Page: Set old Page status to ERASED status */
    if (ErasePage(old_page_address) != true)  return false;

    /* Set new Page status to VALID_PAGE status */
    return FLASH_ProgramHalfWord(new_page_address, ValidPage);
}

template <int m_VarsCount>
bool CFlashEeprom<m_VarsCount>::TestPageErased(void* start_address)
{
    auto ptr = static_cast<TWord*>(start_address);
    for (std::size_t i=0;i<m_PageSize/sizeof(TWord);++i)
        if (*(ptr+i)!=0xFFFF) return false;
    return true;
}

template <int m_VarsCount>
bool CFlashEeprom<m_VarsCount>::Init()
{
    auto page0_status = *(m_AddrPage0);// Get Page0 status
    auto page1_status = *(m_AddrPage1);// Get Page1 status
    /* Get Page1 status */

    /* Check for invalid header states and repair if necessary */
    switch (page0_status){
    case ErasedPage:
        if (page1_status == ValidPage){ /* Page0 erased, Page1 valid */
            /* Erase Page0 */
            return ErasePage(m_AddrPage0);
        }
        else if (page1_status == ReceiveDataPage){ /* Page0 erased, Page1 receive */
            /* Erase Page0 */
            if (ErasePage(m_AddrPage0)!= true) return false;

            /* Mark Page1 as valid */
            return FLASH_ProgramHalfWord(m_AddrPage1, ValidPage);
        }
        else{ /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            return Format();
        }
        break;

    case ReceiveDataPage:
        if (page1_status == ValidPage){ /* Page0 receive, Page1 valid */
            /* Transfer data from Page1 to Page0 */
            for (int VarIdx = 0; VarIdx < m_VarsCount && m_VarsAddr[VarIdx]!=WrongVirtAddr; ++VarIdx){
                int x{-1};
                if (*(m_AddrPage0+3) == m_VarsAddr[VarIdx]) x = VarIdx; // Если переменная уже записана
                if (VarIdx != x){                                       // то, пропускаем этот адрес
                    TData DataVar;
                    /* Read the last variables' updates */
                    /* In case variable corresponding to the virtual address was found */
                    if (ReadVariable(m_VarsAddr[VarIdx], DataVar) == true){
                        /* Transfer the variable to the Page0 */
                        if (VerifyPageFullWriteVariable(m_VarsAddr[VarIdx], DataVar) != TResult::Ok)
                            return false;
                    }
                }
            }
            /* Mark Page0 as valid */
            if (FLASH_ProgramHalfWord(m_AddrPage0, ValidPage) != true) return false;

            /* Erase Page1 */
            return ErasePage(m_AddrPage1);
        }
        else if (page1_status == ErasedPage){ /* Page0 receive, Page1 erased */
            /* Erase Page1 */
            if (ErasePage(m_AddrPage1) != true) return false;
            /* Mark Page0 as valid */
            return FLASH_ProgramHalfWord(m_AddrPage0, ValidPage);
        }
        else{ /* Invalid state -> format eeprom */
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            return Format();
        }
        break;

    case ValidPage:
        if (page1_status == ValidPage){ /* Invalid state -> format eeprom */
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            return Format();
        }
        else if (page1_status == ErasedPage){ /* Page0 valid, Page1 erased */
            /* Erase Page1 */
            return ErasePage(m_AddrPage1);
        }
        else{ /* Page0 valid, Page1 receive */
            /* Transfer data from Page0 to Page1 */
            for (int VarIdx = 0; VarIdx < m_VarsCount && m_VarsAddr[VarIdx]!=WrongVirtAddr; ++VarIdx){
                int x{-1};
                if (*(m_AddrPage1 + 3) == m_VarsAddr[VarIdx]) x = VarIdx; // Если переменная уже записана
                if (VarIdx != x){                                         // то, пропускаем этот адрес
                    TData DataVar;
                    /* Read the last variables' updates */
                    /* In case variable corresponding to the virtual address was found */
                    if (ReadVariable(m_VarsAddr[VarIdx], DataVar) == true){
                        /* Transfer the variable to the Page1 */
                        if (VerifyPageFullWriteVariable(m_VarsAddr[VarIdx], DataVar) != TResult::Ok) return false;
                    }
                }
            }
            /* Mark Page1 as valid */
            if (FLASH_ProgramHalfWord(m_AddrPage1,ValidPage)!= true) return false;
            /* Erase Page0 */
            return ErasePage(m_AddrPage0);
        }
        break;

    default:  /* Any other state -> format eeprom */
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        return Format();
    }
    return true;
}

#endif // FLASH_EEPROM_H
