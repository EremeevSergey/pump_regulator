#include "hw_specific.h"
#include "settings.h"

namespace MCU{

CSettings::CSettings()
    : Sensor1{CSensor::NormalOpen}
    , Sensor2{CSensor::NormalOpen}
    , m_Error{EError::No}
{
}

void CSettings::init()
{
	HW_EepromReservAddress(AddressCopy1);
	HW_EepromReservAddress(AddressCopy2);
	HW_EepromReservAddress(AddressCopy3);
	HW_EepromInit();

}

std::uint16_t CSettings::SensorStateToWord()
{
    std::uint16_t ret{0};
    if (Sensor1==CSensor::NormalClosed) ret|=NormalClosed;
    else                                ret|=NormalOpen;
    if (Sensor2==CSensor::NormalClosed) ret|=NormalClosed << 8;
    else                                ret|=NormalOpen  << 8;
    return ret;
}

bool CSettings::WordToSensorState(uint16_t word)
{
    auto val1 = word & 0xFF;
    auto val2 = word >> 8;
    CSensor::EType s1{CSensor::NormalOpen};
    if (val1==NormalClosed) s1 = CSensor::NormalClosed;
    else if (val1!=NormalOpen) return false;
    CSensor::EType s2{CSensor::NormalOpen};
    if (val2==NormalClosed) s2 = CSensor::NormalClosed;
    else if (val2!=NormalOpen) return false;
    Sensor1 = s1;
    Sensor2 = s2;
    return true;
}

void CSettings::resetToDefault()
{
    Sensor1 = CSensor::NormalOpen;
    Sensor2 = CSensor::NormalOpen;
}

bool CSettings::load()
{
    std::uint16_t read1{0},read2{0},read3{0},read{0};
    if (HW_EepromRead(AddressCopy1,read1) &&
            HW_EepromRead(AddressCopy2,read2) &&
            HW_EepromRead(AddressCopy3,read3)){
        if (read1==read2) read = read1;
        if (read2==read3) read = read2;
        if (read3==read1) read = read3;
        if (WordToSensorState(read)) return true;
    }
    HW_EepromClear();
    resetToDefault();
    return save();
}

bool CSettings::save()
{
    auto to_save = SensorStateToWord();
    if (HW_EepromWrite(AddressCopy1,to_save) &&
            HW_EepromWrite(AddressCopy2,to_save) &&
            HW_EepromWrite(AddressCopy3,to_save)) return true;
    HW_EepromClear();
    // Ещё раз попробуем
    if (HW_EepromWrite(AddressCopy1,to_save) &&
            HW_EepromWrite(AddressCopy2,to_save) &&
            HW_EepromWrite(AddressCopy3,to_save)) return true;
    HW_EepromClear();
    resetToDefault();
    m_Error = EError::LoadSaveError;
    return false;
}

}// namespace MCU
