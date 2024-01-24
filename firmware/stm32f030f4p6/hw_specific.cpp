#include <cstdint>
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_cortex.h"
#include "system/config.h"
#include "system/hw_specific.h"
#include "system/settings.h"
#include "eeprom/src/flash_eeprom.h"


//============================================================================//
//                              Система глобально                             //
//============================================================================//
void SystemClock_Config(void){
	  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void HW_MkInit(){
	  HAL_Init();
    SystemClock_Config();
	  SystemCoreClockUpdate();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
}

//============================================================================//
//                              Цифровые порты                                //
//============================================================================//

namespace {

using namespace MCU;

TPortBit pins_to_wh[]={
		{NoPort, 0},// 1 pin
		{GPIOF , GPIO_PIN_0},// 2 pin
		{GPIOF , GPIO_PIN_1},// 3 pin
		{NoPort, 0},// 4 pin
		{NoPort, 0},// 5 pin
		{GPIOA , GPIO_PIN_0},// 6 pin
		{GPIOA , GPIO_PIN_1},// 7 pin
		{GPIOA , GPIO_PIN_2},// 8 pin
		{GPIOA , GPIO_PIN_3},// 9 pin
		{GPIOA , GPIO_PIN_4},// 10 pin
		{GPIOA , GPIO_PIN_5},// 11 pin
		{GPIOA , GPIO_PIN_6},// 12 pin
		{GPIOA , GPIO_PIN_7},// 13 pin
		{GPIOB , GPIO_PIN_1},// 14 pin
		{NoPort, 0},// 15 pin
		{NoPort, 0},// 16 pin
		{GPIOA , GPIO_PIN_9},// 17 pin
		{GPIOA ,GPIO_PIN_10},// 18 pin
		{GPIOA ,GPIO_PIN_13},// 19 pin
//		{NoPort, 0},// 19 pin //!< \todo заменить на предыдущий
		{GPIOA ,GPIO_PIN_14} // 20 pin
};

}

void HW_PinToPortdata(int pin,MCU::TPort& dst_port,MCU::TBit& dst_bit){
	if (pin>0 && static_cast<unsigned>(pin)<=sizeof(pins_to_wh)/sizeof(MCU::TPortBit)){
		dst_port = pins_to_wh[pin-1].Port;
		dst_bit  = pins_to_wh[pin-1].Bit;
	}
	else{
		dst_port = MCU::NoPort;
		dst_bit  = 0;
	}
}


/*!
 * \brief Настроить соответствующий вывод порта
 * \param bit    вывод порта
 * \param config конфигурация вывода порта
 * \return \b true если всё прошло успешно, иначе \b false
 */
bool HW_InitPortBit (MCU::TPort port, MCU::TBit bit,int config,int pull){
	if (port!=MCU::NoPort){
		GPIO_InitTypeDef GPIO_InitStruct {0,0,0,0,0};
		GPIO_InitStruct.Pin = bit;
		switch(config){
		case  PORT_OUTPUT_PP: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; break;
		case  PORT_OUTPUT_OD: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; break;
		case  PORT_ALTERNATE: GPIO_InitStruct.Mode = GPIO_MODE_AF_OD    ; break;
		case  PORT_ANALOG   : GPIO_InitStruct.Mode = GPIO_MODE_ANALOG   ; break;
		default:              GPIO_InitStruct.Mode = GPIO_MODE_INPUT    ; break;
		}

		GPIO_InitStruct.Pull=GPIO_NOPULL;
		if (pull & PORT_PULL_UP  ) GPIO_InitStruct.Pull|=GPIO_PULLUP;
		if (pull & PORT_PULL_DOWN) GPIO_InitStruct.Pull|=GPIO_PULLDOWN;

		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init((GPIO_TypeDef*)port, &GPIO_InitStruct);
		return true;
	}
	return false;
}

void HW_SetPortBit  (MCU::TPort port, MCU::TBit bit){
	if (port!=MCU::NoPort)
		HAL_GPIO_WritePin((GPIO_TypeDef*)port, bit, GPIO_PIN_SET);
}

void HW_ResetPortBit(MCU::TPort port, MCU::TBit bit){
	if (port!=MCU::NoPort)
		HAL_GPIO_WritePin((GPIO_TypeDef*)port, bit, GPIO_PIN_RESET);
}

MCU::EPortState HW_GetPortBit(MCU::TPort port, MCU::TBit bit){
	if (port!=MCU::NoPort &&
			HAL_GPIO_ReadPin((GPIO_TypeDef*)port, bit)==GPIO_PIN_SET) return MCU::On;
	else return MCU::Off;
}

//============================================================================//
//                                Системый таймер                             //
//============================================================================//
int HW_MainSystemTimerFlag=0;

extern "C" void
SysTick_Handler (void)
{
	HAL_IncTick();
	HW_MainSystemTimerFlag++;
}

bool HW_TimerInit (){
	return true;
}

//============================================================================//
//                                     EEPROM                                 //
//============================================================================//
/*!
 * \brief Функция, стирающая странице во внутренней флэш-памяти.
 * \details Её надо где-то в проекте определить (на базе STL, HAL, CMSIS...)
 * \param addr - адрес начала стираемой страницы
 * \return \b true  в случае удачи, иначе \b false
 */
bool FLASH_ErasePage(void* addr){
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef str{FLASH_TYPEERASE_PAGES,reinterpret_cast<std::uint32_t>(addr),1};
	uint32_t PageError;
	bool ret = HAL_FLASHEx_Erase(&str,&PageError)==HAL_OK;
	HAL_FLASH_Lock();
	return ret;
}

/*!
 * \brief Функция, записывающая во внутреннюю флэш-память полу-слово (16 бит)
 * \param address Адрес во флэш-памяти
 * \param data    аписываемое значение (16 бит)
 * \return \b true  в случае удачи, иначе \b false
 */
bool FLASH_ProgramHalfWord(void* address,std::uint16_t data){
	HAL_FLASH_Unlock();
	bool ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, reinterpret_cast<std::uint32_t>(address), data)==HAL_OK;
	HAL_FLASH_Lock();
	return ret;
}

void* EEPROM_START_ADDRESS = reinterpret_cast<void*>(0x08003800); // Начальный адрес EEPROM
constexpr int    PAGE_SIZE = 0x0400; // Размер страницы 1 кбайт
CFlashEeprom<MCU::CSettings::AddressCount> EEPROM(EEPROM_START_ADDRESS,PAGE_SIZE);

void HW_EepromInit  (){
	EEPROM.Init();
}

void HW_EepromClear (){
	EEPROM.Format();
}

void HW_EepromReservAddress(std::uint16_t address){
	EEPROM.AddVirtualAddrress(address);
}

bool HW_EepromRead (std::uint16_t address,std::uint16_t& data){
	return EEPROM.ReadVariable(address,data);
}

bool HW_EepromWrite(std::uint16_t address,std::uint16_t  data){
	return EEPROM.WriteVariable(address,data);
}



