#include <fstream>
#include <algorithm>
#include "veeprom.h"

namespace GUI {
namespace{
    constexpr auto filename="veeprom.txt";
}// namespace

CVirtualEEPROM::CVirtualEEPROM()
{

}

CVirtualEEPROM::~CVirtualEEPROM()
{
    saveToFile();
}

void CVirtualEEPROM::saveToFile()
{
    std::ofstream stream(filename);
    for (auto& val:m_Values)
        stream<<val.Address<<'='<<val.Value<<std::endl;
}

void CVirtualEEPROM::init ()
{
    constexpr std::uint16_t FFFF = 0xFFFF;
    m_Values.clear();
    std::ifstream stream(filename);
    while (stream){
        TValue val{FFFF,FFFF};
        char ch{0};
        stream >> val.Address >> ch >> val.Value;
        if (stream && val.Address!=FFFF && val.Value!=FFFF && ch=='='){
            m_Values.push_back(val);
        }
    }
}

bool CVirtualEEPROM::Read (std::uint16_t address,std::uint16_t& data)
{
    auto val = std::find_if(m_Values.begin(),m_Values.end(),[address](auto val){return val.Address==address;});
    if (val!=m_Values.end()){
        data = (*val).Value;
        return true;
    }
    return false;
}

bool CVirtualEEPROM::Write(std::uint16_t address,std::uint16_t  data)
{
    auto val = std::find_if(m_Values.begin(),m_Values.end(),[address](auto val){return val.Address==address;});
    if (val!=m_Values.end()){
        (*val).Value = data;
        saveToFile();
        return true;
    }
    else{
        m_Values.push_back({address,data});
        saveToFile();
        return true;
    }
}

}//namespace GUI
