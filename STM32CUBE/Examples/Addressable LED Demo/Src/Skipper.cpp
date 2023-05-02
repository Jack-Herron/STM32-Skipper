/*
 * Skipper.cpp
 *
 *  Created on: Feb. 25, 2023
 *      Author: jackherron
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include <Skipper.h>

Clock clock;

uint32_t Clock::getCoreTimerFreq(){
	return coreTimerFreq;
}

uint32_t Clock::getCoreFreq(){
	return coreFreq;
}

uint32_t Clock::getAPB2PeripheralFreq(){
	return APB2PeripheralFreq;
}

uint32_t Clock::getAPB2TimerFreq(){
	return APB2TimerFreq;
}

uint32_t Clock::getAPB1PeripheralFreq(){
	return APB1PeripheralFreq;
}

uint32_t Clock::getAPB1TimerFreq(){
	return APB1TimerFreq;
}

void Clock::updateValues(){
	uint8_t tim1Multiplier;
	uint8_t tim2Multiplier;

	if(APB1 == 1){	tim1Multiplier = 1;	}
	else{			tim1Multiplier = 2;	}

	if(APB2 == 1){	tim2Multiplier = 1;	}
	else{			tim2Multiplier = 2;	}

	SysClk = (16000000/M*N/P);
	HClk = SysClk/AHB;
	coreTimerFreq = HClk;
	coreFreq = HClk;
	APB1PeripheralFreq = HClk/APB1;
	APB1TimerFreq = HClk/APB1*tim1Multiplier;
	APB2PeripheralFreq = HClk/APB2;
	APB2TimerFreq = HClk/APB2*tim2Multiplier;
}


void SysTick_Handler() {

}

void Clock::setParams(uint8_t setM,uint8_t setN,uint8_t setP,uint8_t setQ,uint8_t setAHB,uint16_t setAPB1,uint8_t setAPB2){
	M = setM;
	N = setN;
	P = setP;
	Q = setQ;
	AHB = setAHB;
	APB1 = setAPB1;
	APB2 = setAPB2;
}

void Clock::init(){

	uint8_t newP = (P-2)/2;
	uint8_t newAHB;
	uint8_t newAPB1;
	uint8_t newAPB2;

	switch(AHB){
		case 1:
			newAHB = 0b0000;
			break;
		case 2:
			newAHB = 0b1000;
			break;
		case 4:
			newAHB = 0b1001;
			break;
		case 8:
			newAHB = 0b1010;
			break;
		case 16:
			newAHB = 0b1011;
			break;
		case 64:
			newAHB = 0b1100;
			break;
		case 128:
			newAHB = 0b1101;
			break;
		case 256:
			newAHB = 0b1110;
			break;
		case 512:
			newAHB = 0b1111;
			break;
	}

	switch(APB1){
		case 1:
			newAPB1 = 0b000;
		break;

		case 2:
			newAPB1 = 0b100;
		break;

		case 4:
			newAPB1 = 0b101;
		break;

		case 8:
			newAPB1 = 0b110;
		break;

		case 16:
			newAPB1 = 0b111;
		break;

	}

	switch(APB2){
		case 1:
			newAPB2 = 0b000;
		break;

		case 2:
			newAPB2 = 0b100;
		break;

		case 4:
			newAPB2 = 0b101;
		break;

		case 8:
			newAPB2 = 0b110;
		break;

		case 16:
			newAPB2 = 0b111;
		break;

	}

	RCC->PLLCFGR = 0x00000000;																	//reset PLL register
	RCC -> CR |= RCC_CR_HSEON;																	//turn on HSE
	while(!(RCC -> CR & RCC_CR_HSERDY)){} 														//wait for HSE confirmation
	PWR ->CR |= PWR_CR_VOS; 																	// set up power
	FLASH -> ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;	//Set up flash
	RCC->PLLCFGR |= (Q << 24) | (newP << 16) | (N << 6) | (M << 0);								// set PLL
	RCC->PLLCFGR |= (1<<22);
	RCC -> CR &= ~(1<<0);
	RCC -> CFGR |= (2 << 0) | (newAHB << 4) | (newAPB1<< 10) | (newAPB2<< 13);
	RCC -> CFGR |= (0 << 24);
	RCC -> CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	RCC -> CFGR |= RCC_CFGR_SW_PLL;
	while((RCC -> CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
	updateValues();
	SysTick->LOAD = (HClk/1000)-1;
	SysTick->CTRL |= (1<<2);

}

void delay_ms(uint32_t ms){
	SysTick->CTRL = 0b000101;
	for(uint32_t i = 0; i < ms;	i++){
		while(!(SysTick->CTRL & (1<<16)));
	}
	SysTick->CTRL = 0;
}




