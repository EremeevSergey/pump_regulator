#ifndef VEEPROM_H
#define VEEPROM_H
#include <cstdint>
#include <vector>
namespace GUI {

class CVirtualEEPROM
{
public:
    CVirtualEEPROM();
    CVirtualEEPROM(const CVirtualEEPROM&)=delete;
    CVirtualEEPROM(      CVirtualEEPROM&&)=delete;
    CVirtualEEPROM& operator=(const CVirtualEEPROM&)=delete;
    CVirtualEEPROM& operator=(      CVirtualEEPROM&&)=delete;
    ~CVirtualEEPROM();

    void init ();
    void clear(){m_Values.clear();}
    bool Read (std::uint16_t address,std::uint16_t& data);
    bool Write(std::uint16_t address,std::uint16_t  data);
private:
    struct TValue{
        std::uint16_t Address;
        std::uint16_t Value;
    };
    std::vector<TValue> m_Values;

    void saveToFile();
};

}//namespace GUI
#endif // VEEPROM_H
