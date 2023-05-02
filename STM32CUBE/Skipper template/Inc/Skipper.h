/*
 * Skipper.h
 *
 *  Created on: Feb 17, 2023
 *      Author: jackherron
 */

//uint8_t

#ifndef SKIPPER_H_
#define SKIPPER_H_

void clock_init(uint8_t M,uint8_t N,uint8_t P,uint8_t Q,uint32_t AHB,uint8_t APB1,uint8_t APB2){

	P = (P-2)/2;

	switch(AHB){
		case 1:
			AHB = 0b0000;
			break;
		case 2:
			AHB = 0b1000;
			break;
		case 4:
			AHB = 0b1001;
			break;
		case 8:
			AHB = 0b1010;
			break;
		case 16:
			AHB = 0b1011;
			break;
		case 64:
			AHB = 0b1100;
			break;
		case 128:
			AHB = 0b1101;
			break;
		case 256:
			AHB = 0b1110;
			break;
		case 512:
			AHB = 0b1111;
			break;
	}

	switch(APB1){
		case 1:
			APB1 = 0b000;
		break;

		case 2:
			APB1 = 0b100;
		break;

		case 4:
			APB1 = 0b101;
		break;

		case 8:
			APB1 = 0b110;
		break;

		case 16:
			APB1 = 0b111;
		break;

	}

	switch(APB2){
		case 1:
			APB2 = 0b000;
		break;

		case 2:
			APB2 = 0b100;
		break;

		case 4:
			APB2 = 0b101;
		break;

		case 8:
			APB2 = 0b110;
		break;

		case 16:
			APB2 = 0b111;
		break;

	}



	RCC->PLLCFGR = 0x00000000;//reset PLL register

	RCC -> CR |= RCC_CR_HSEON;
	while(!(RCC -> CR & RCC_CR_HSERDY)){} //turn on HSE and wait for confirmation

	PWR ->CR |= PWR_CR_VOS; // set up power
	FLASH -> ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;//Set up flash

	RCC->PLLCFGR |= (Q << 24) | (P << 16) | (N << 6) | (M << 0);// set PLL
	RCC->PLLCFGR |= (1<<22);
	RCC -> CFGR |= (2 << 0); // switch to
	RCC -> CFGR |= (AHB << 4);
	RCC -> CFGR |= (APB1<< 10);
	RCC -> CFGR |= (APB2<< 13);
	RCC -> CFGR |= (0 << 24);

	RCC -> CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){}

	RCC -> CFGR |= RCC_CFGR_SW_PLL;
	while((RCC -> CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
	//RCC -> CR &= !(1<<0);
}

class I2C{
public:
	I2C_TypeDef * port = I2C1;				// port to be used
	uint32_t APBfreq = 40000000;			// defines APB clock speed(Jack please remove this)
	uint32_t freq = 100000;					// set I2C freq
	uint32_t riseTime = 1000; 				// max rise time in nS
	uint8_t fm = 0;							// fast mode
	uint8_t duty = 0;						// set duty cycle
	uint8_t mode = 0;						// 0 = master mode, 1 = slave mode
	uint8_t slaveAddress = 0b10011001; 			// slave address

	void set_port(I2C_TypeDef * i){			// this function is used to change the port variable
		port = i;
	}

	void setGPIO(){							// this function sets up the I2C GPIO pins
		if (port == I2C1){
			RCC->AHB1ENR |= (1<<1); // enable GPIOB clock
			GPIOB->MODER |= (2 << 16);
			GPIOB->MODER |= (2 << 18);

			GPIOB->OTYPER |= (1<<8);
			GPIOB->OTYPER |= (1<<9);

			GPIOB->AFR[1] |= (4<<0);
			GPIOB->AFR[1] |= (4<<4);
		}else if(port == I2C2){
			RCC->AHB1ENR |= (1<<5); // enable GPIOF clock
			GPIOF->MODER |= (2 << 2);
			GPIOF->MODER |= (2 << 0);

			GPIOF->OTYPER |= (1<<1);
			GPIOF->OTYPER |= (1<<0);

			GPIOF->AFR[0] |= (4<<0);
			GPIOF->AFR[0] |= (4<<4);
		}
	}
	void init(){
		uint8_t ppc;
		if(port == I2C1){
			RCC->APB1ENR |= (1<<21); // enable I2C1 clock
		}else if(port == I2C2){
			RCC->APB1ENR |= (1<<22); // enable I2C2 clock
		}

		if(fm == 1){
			port->CCR |= (1<<15);
			if(duty == 1){
				port->CCR |= (1<<14);
				ppc = 25;
			}else{ppc = 3;}
		}else{ppc = 2;};

		uint32_t ccrValue = (1000000000/freq/ppc)/(1000000000/APBfreq);
		port->CR2 |= ((APBfreq/1000000)<<0); //set Mhz
		port->CCR |= (ccrValue<<0); //set 100Khz
		port->TRISE |= (((riseTime/(1000000000/APBfreq))+1)<<0); //set rise times

		port->CR1 |= (1<<0);//enable I2C
	}
	void reset(){
		port->CR1 |= 0;
	}
	void write(unsigned char data){
		while (!(port->SR1 & (1<<7)));
		port->DR = data;
		while (!(port->SR1 & (1<<2)));
	}
	void address(unsigned char addr){
		port->DR = addr;
		while (!(port->SR1 & (1<<1)));
		uint8_t temp [[maybe_unused]] = port->SR1 | port->SR2; // clear registers
	}


	unsigned char read(int ack){
		if (ack) port->CR1 |= (1<<10);
			else port->CR1 &= ~(1<<10);
		while(!(port->SR1 & (1<<6)));
		return (port->DR);
	}

	void stop(void){
		port->CR1 |= (1<<9);
		while((port->SR2 & (1<<1)));
	}

	void start(void){
		port->CR1 |= (1<<8);
	 	while(!(port->SR1 & (1<<0)));
	}
};

class SPI{
public:

};

class USART{
public:

};

class CAN{
public:

};

class USB{
public:

};

class SD{
public:

};

#endif /* SKIPPER_H_ */
