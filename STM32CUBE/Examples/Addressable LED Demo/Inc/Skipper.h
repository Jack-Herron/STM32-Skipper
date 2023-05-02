/*
 * Skipper.h
 *
 *  Created on: Feb 17, 2023
 *      Author: jackherron
 */

//uint8_t

#ifndef SKIPPER_H_
#define SKIPPER_H_

class Clock{
protected:
	uint32_t APB1TimerFreq;						// stores the value of APB1 timer frequency in hz
	uint32_t APB1PeripheralFreq;				// stores the value of APB1 peripheral frequency in hz
	uint32_t APB2TimerFreq;						// stores the value of APB2 timer frequency in hz
	uint32_t APB2PeripheralFreq;				// stores the value of APB2 peripheral frequency in hz
	uint32_t coreFreq;							// stores the value cortex system clock frequency in hz
	uint32_t coreTimerFreq;						// stores the value of cortex system timer frequency in hz;

	uint32_t SysClk;
	uint32_t HClk;

	void updateValues();

public:

	uint8_t M;
	uint8_t N;
	uint8_t P;
	uint8_t Q;
	uint16_t AHB;
	uint8_t APB1;
	uint8_t APB2;


	void setParams(uint8_t setM,uint8_t setN,uint8_t setP,uint8_t setQ,uint8_t setAHB,uint16_t setAPB1,uint8_t setAPB2);

	void init();
	uint32_t getAPB1TimerFreq();
	uint32_t getAPB1PeripheralFreq();
	uint32_t getAPB2TimerFreq();
	uint32_t getAPB2PeripheralFreq();
	uint32_t getCoreFreq();
	uint32_t getCoreTimerFreq();

};

void delay_ms(uint32_t ms);

class I2C{
private:
	I2C_TypeDef * port = I2C1;				// port to be used
	uint8_t slaveAddress = 0b10011001;		// slave address
public:

	uint32_t APBfreq = 40000000;			// defines APB clock speed(Jack please remove this)
	uint32_t freq = 100000;					// set I2C freq
	uint32_t riseTime = 1000; 				// max rise time in nS
	uint8_t fm = 0;							// fast mode
	uint8_t duty = 0;						// set duty cycle
	uint8_t mode = 0;						// 0 = master mode, 1 = slave mode
	uint8_t inqEn = 0;

	void setPort(I2C_TypeDef * i){			// this function is used to change the port variable
		port = i;
	}

	void setAddress(uint8_t addr){			// this function is used to change the slave address variable
		slaveAddress = addr;
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
			if(inqEn){
				//enable interrupt for I2C1
			}
		}else if(port == I2C2){
			RCC->APB1ENR |= (1<<22); // enable I2C2 clock
			if(inqEn){
				//enable interrupt for I2C2
			}
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
	SPI_TypeDef * port = SPI1;
	uint8_t BR = 0;
	uint8_t mode = 0;
	uint8_t MSTR = 1;

	void setPort(SPI_TypeDef * i){			// this function is used to change the port variable
		port = i;
	}

	void setGPIO(){
		if(port == SPI1){
			RCC->AHB1ENR |= (1<<0); // enable GPIOA

			GPIOA->MODER |= (2 << 10);
			GPIOA->MODER |= (2 << 12);
			GPIOA->MODER |= (2 << 14);

			GPIOA->OSPEEDR |= (3 << 10);
			GPIOA->OSPEEDR |= (3 << 12);
			GPIOA->OSPEEDR |= (3 << 14);

			GPIOA->OTYPER &= ~(1<<5);
			GPIOA->OTYPER &= ~(1<<6);
			GPIOA->OTYPER &= ~(1<<7);

			GPIOA->AFR[0] |= (5<<20);
			GPIOA->AFR[0] |= (5<<24);
			GPIOA->AFR[0] |= (5<<28);
		}else if(port == SPI3){
			RCC->AHB1ENR |= (1<<1); // enable GPIOB

			GPIOB->MODER |= (2 << 6);
			GPIOB->MODER |= (2 << 8);
			GPIOB->MODER |= (2 << 10);

			GPIOB->OSPEEDR |= (3 << 6);
			GPIOB->OSPEEDR |= (3 << 8);
			GPIOB->OSPEEDR |= (3 << 10);

			GPIOB->OTYPER &= ~(1<<3);
			GPIOB->OTYPER &= ~(1<<4);
			GPIOB->OTYPER &= ~(1<<5);

			GPIOB->AFR[0] |= (6<<12);
			GPIOB->AFR[0] |= (6<<16);
			GPIOB->AFR[0] |= (6<<20);
		}
	}

	void init(void){
		if(port == SPI1){
			RCC->APB2ENR |= (1<<12); 						// enable SPI_1 clock
		}else if(port == SPI3){
			RCC->APB1ENR |= (1<<15); 						// enable SPI_3 clock
		}
		port->CR1 |= (mode<<0 | MSTR<<2 | BR<<3 | 3<<8);	//Configure SPI
		port->CR1 |= (1<<6); 								//Enable SPI
	}

	void transmit(uint8_t * data, int size){
		uint8_t temp [[maybe_unused]];

		for(int i = 0; i<size; i++){
			while(!((port->SR) & (1<<1))){}; // wait for TX buffer to be empty
			port->DR = data[i];
		}

		//while(!((SPI1->SR) & (1<<1))); // wait for TX buffer to be empty
		while((port->SR) & (1<<7)); // wait for busy to clear

		temp = port->DR;
		temp = port->SR;
	}

	uint32_t receive(uint8_t add, int size){
		uint8_t temp [[maybe_unused]];
		uint32_t message = 0;

		while(!((port->SR) & (1<<1))){}; 		// wait for TX buffer to be empty
		port->DR = add;							// send address
		while(!((port->SR) & (1<<0))){}; 		// wait for read data to be ready
		temp = port->DR;						// read-clear data register
		for(int i = size-2; i>=0; i--){
			while(!((port->SR) & (1<<1))){}; 	// wait for TX buffer to be empty
			port->DR = 0; 						// send dummy data
			while(!((port->SR) & (1<<0))){}; 	// wait for read data to be ready
			message |= ((port->DR)<<(8*i)); 	// copy read data to message
		}
		while(!((port->SR) & (1<<1)));			// wait for TX buffer to be empty
		while((port->SR) & (1<<7)); 			// wait for busy to clear
		temp = port->SR; 						// clear ST
		return (message);
	}
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

extern Clock clock;

#endif /* SKIPPER_H_ */

