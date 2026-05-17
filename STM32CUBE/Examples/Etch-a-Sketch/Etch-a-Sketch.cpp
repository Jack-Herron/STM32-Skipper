
/*
 * date [Dec 5th]
 * Project name - [Etch-A-Sketch]
 * Created by: [Jack Herron]
 *
 * description[This file controls the stepper motors for the Etch a Sketch video]
 */

#include <stdint.h>													// include standard c functions
#include <stm32f4xx.h>												// include MCU specific definitions
#include <Skipper.h>												// include skipper library
#include <cmath>
//store the float array below
const uint32_t dataArrayLength = 155;
float printData[155][2] = {{10.701,60.5617},{10.701,27.0961},{19.4033,27.0961},{19.4033,39.0643},{22.0078,39.0643},{22.0078,27.0961},{30.7101,27.0961},{30.7101,60.4772},{39.5039,60.5111},{37.4221,59.3479},{36.5063,58.5927},{35.7537,57.7505},{34.715,55.916},{34.2034,53.9265},{34.0174,51.0842},{33.9554,46.6918},{33.9554,40.966},{34.0122,36.6614},{34.1827,33.8347},{34.6788,31.8193},{35.671,29.9899},{37.149,28.4552},{39.061,27.3234},{41.3658,26.6258},{44.0219,26.3933},{44.0633,32.057},{43.4587,32.1914},{43.0091,32.5944},{42.7455,33.628},{42.6577,35.695},{42.6577,51.322},{42.7145,53.6784},{42.885,54.9186},{43.2881,55.4302},{44.0013,55.6008},{44.7299,55.4044},{45.1381,54.8153},{45.3242,53.4872},{45.3862,51.0739},{45.3862,35.695},{45.319,33.783},{45.1175,32.7185},{44.7553,32.2389},{44.1408,32.0586},{44.104,26.3935},{46.6064,26.6223},{48.8382,27.2821},{50.776,28.3725},{51.5861,29.0843},{52.2901,29.9073},{53.3288,31.7418},{53.8404,33.7313},{54.0265,36.5735},{54.0885,40.966},{54.0885,46.6918},{54.0265,51.0016},{53.8404,53.8438},{53.3547,55.8436},{52.3522,57.6678},{51.6167,58.5095},{50.6984,59.2791},{48.6002,60.4874},{59.4783,60.5477},{55.5974,27.0961},{64.0517,27.0961},{64.9405,38.7749},{65.85,50.0404},{66.6872,39.9325},{68.2478,27.0961},{77.3015,27.0961},{78.2524,38.1341},{79.2032,50.8465},{80.1696,38.8421},{81.6217,27.0961},{90.0759,27.0961},{86.2217,60.5644},{92.3704,60.537},{92.3704,53.8644},{92.3704,52.1488},{92.3704,42.9504},{99.9771,42.9504},{100.078,43.7824},{100.339,44.3767},{100.76,44.7332},{101.341,44.8521},{102.184,44.6454},{102.644,44.0253},{102.861,42.5731},{102.933,39.9118},{102.933,34.3514},{102.866,32.8166},{102.664,31.8916},{102.287,31.411},{101.651,31.2508},{100.871,31.4627},{100.432,32.0983},{100.23,33.4523},{100.163,35.8604},{100.163,37.8861},{91.8123,37.8861},{92.019,34.0775},{92.5564,31.4576},{93.0099,30.477},{93.6674,29.561},{94.5291,28.7097},{95.595,27.9229},{96.8533,27.2537},{98.2718,26.7757},{99.8505,26.4889},{101.589,26.3933},{104.049,26.6},{106.178,27.2201},{107.94,28.2123},{109.3,29.5352},{110.302,31.0648},{110.995,32.7185},{111.398,34.9199},{111.532,38.0928},{111.532,42.1856},{111.403,44.7022},{111.015,46.6711},{110.681,47.4927},{110.194,48.2627},{108.762,49.6476},{107.856,50.2083},{106.876,50.6088},{105.82,50.8491},{104.69,50.9292},{103.268,50.7806},{102.039,50.3349},{101.004,49.5921},{100.163,48.5521},{100.163,52.1488},{92.3677,52.1517},{92.3817,53.8703},{100.163,53.8644},{100.163,60.5617},{92.3704,60.5617},{86.2105,60.5617},{75.3378,60.5617},{73.9374,52.3245},{72.7127,43.0331},{70.2942,60.5617},{59.4835,60.5617},{48.5241,60.5209},{46.4135,61.0786},{44.0219,61.2645},{41.7403,61.0931},{39.6707,60.579},{30.7101,60.5617},{22.0078,60.5617},{22.0078,46.5057},{19.4033,46.5057},{19.4033,60.5617},{10.7756,60.5617}};
//-------------------------

SPI tmcCom;															// begin an SPI instance, and name it tmcCom (see Skipper Library for more)

//Config
float backlash = 1.5;												// travel move inserted at every change in direction to cancel backlash
const uint8_t stepResolution = 0;									// stepper resolution (0-8) (256,128,64,32,16,8,4,2,1)
const uint8_t mmPerSecond = 10; 									// stepper speed
const uint8_t speedLimit = 255; 									// stepper speed limit
const uint8_t RunCurrent = 16; 										// stepper run current (0-31)
const uint8_t toff = 15;											// stepper toff value
const uint8_t HoldCurrent = 10; 									// stepper hold current (0-31)
const float stepsPerMM[2] = {20.4,20.4};    						// steps per mm


//Stepper addresses (from TMC2130 datasheet)
const uint8_t IHOLD_IRUN = 0x10;
const uint8_t GCONF = 0x00;
const uint8_t GSTAT = 0x01;
const uint8_t IOIN = 0x04;
const uint8_t TSTEP = 0x12;
const uint8_t TPWMTHRS = 0x13;
const uint8_t TCOOLTHRS = 0x14;
const uint8_t THIGH = 0x15;
const uint8_t XDIRECT = 0x2D;
const uint8_t VDCMIN = 0x33;
const uint8_t CHOPCONF = 0x6C;
const uint8_t COOLCONF = 0x6D;
const uint8_t DCCTRL = 0x6E;
const uint8_t DRV_STATUS = 0x6F;
const uint8_t PWMCONF = 0x70;
const uint8_t PWM_SCALE = 0x71;
const uint8_t LOST_STEPS = 0x73;

//program variables
const uint16_t realStepResolution = (pow(2,(8-stepResolution)));	// real step resolution used for distance/speed calculations
float currentX = 0;													// stores the current X location in mm
float currentY = 0;													// stores the current Y location in mm
uint32_t stepsRemaining[2];											// used to reload overflowed times with remainder
uint8_t busyY = 0;													// Y motor busy flag
uint8_t busyX = 0;													// X motor busy flag


void GPIO_init(void){	// this is called to initialize all GPIO pins
	RCC->AHB1ENR |= (1<<3); 										// enable GPIOD clock
	RCC->AHB1ENR |= (1<<2); 										// enable GPIOC clock
	RCC->AHB1ENR |= (1<<0); 										// enable GPIOA clock
	//Enable on board LED

	GPIOD->MODER |= (1<<8); 										// set PD4 to output
	GPIOD->ODR |= (1<<4);											// set PD4 HIGH

	//Driver 0 Y
	GPIOA->MODER |= (1<<0);											//Set CS(PA0) Pin to Output
	GPIOA->ODR |= (1<<0);											//Set CS(PA0) Pin to HIGH

	GPIOC->MODER |= (1<<2);											//Set Dir(PC1) Pin to Output
	GPIOC->ODR |= (1<<1);											//Set Dir(PC1) Pin to HIGH

	GPIOA->MODER |= (2<<2);											//Set Step(PA1) Pin to Alt function
	GPIOA->AFR[0] |= (1<<4);										//Set Step(PA1) Pin to AF1 (TIM2_CH2)

	//Driver 1 X
	GPIOC->MODER |= (1<<0);											//Set CS(PC0) Pin to Output
	GPIOC->ODR |= (1<<0);											//Set CS(PC0) Pin to HIGH

	GPIOC->MODER |= (1<<4); 										//Set Dir(PC2) Pin to Output
	GPIOC->ODR |= (1<<2);											//Set Dir(PC2) Pin to LOW

	GPIOA->MODER |= (2<<4);											//Set Step(PA2) Pin to Alt function
	GPIOA->AFR[0] |= (2<<8);										//Set Step(PA2) Pin to AF2 (TIM5_CH3)
}

void TIM2_init(){													// this timer sets PWM cycle width to control stepper speed (This is connected to PA1 to drive X step pulse train)
	RCC->APB1ENR |= (1<<0);											// enable Tim2 clock
	TIM2->CR2 |= (2<<4);											// send pulse (to slave timer) when update (when CNT == ARR)
	TIM2->CCMR1 |= (6<<12);											// set CC mode to PWM output
	TIM2->CCR2 |= (clock.getAPB1TimerFreq()/(speedLimit * uint32_t(stepsPerMM[1]) * realStepResolution));		// set pulse width (also sets speed limit, effectively by providing a pulse time)
	TIM2->CCER |= (1<<4);											// enable output to T2C2 pin
	TIM2->ARR = 0;													// clear auto reload register
	TIM2->SR = 0;													// clear any status flags
}

void TIM5_init(){													// this timer sets PWM cycle width to control stepper speed (This is connected to PA2 to drive Y step pulse train)
	RCC->APB1ENR |= (1<<3);											// enable Tim2 clock
	TIM5->CR2 |= (2<<4);											// send pulse (to slave timer) when update (when CNT == ARR)
	TIM5->CCMR2 |= (6<<4);											// set CC mode to PWM output
	TIM5->CCR3 |= (clock.getAPB1TimerFreq()/(speedLimit * uint32_t(stepsPerMM[0]) * realStepResolution));		// set pulse width (also sets speed limit, effectively by providing a pulse time)
	TIM5->CCER |= (1<<8);											// enable output to T5C3 pin
	TIM5->ARR = 0;													// clear auto reload register
	TIM5->SR = 0;													// clear any status flags
}


void TIM3_init(){ 													//Slave timer (mstep counter for timer 5)
	NVIC_SetPriority (TIM3_IRQn, 1);  								// Set interrupt Priority
	NVIC_EnableIRQ (TIM3_IRQn);										// enable interrupt
	RCC->APB1ENR |= (1<<1);											// enable TIM3 clock
	TIM3->SMCR |= (2<<4);											// selects trigger source ITR2(TIM5) (selects what source increments the counter)
	TIM3->SMCR |= (7<<0);											// slave mode selection (External Clock mode 1) (counts up on rising edge of trigger source)
	TIM3->DIER |= (1<<0);											// enable interrupt request on update (sends interrupt when CNT == ARR)
	TIM3->SR = 0;													// clear any status flags
}

void TIM4_init(){													// Slave timer (mstep counter for timer 2)
	NVIC_SetPriority (TIM4_IRQn, 0);								// Set interrupt Priority
	NVIC_EnableIRQ (TIM4_IRQn);										// enable interrupt
	RCC->APB1ENR |= (1<<2);											// enable TIM4 clock
	TIM4->SMCR |= (1<<4);											// selects trigger source ITR2(TIM5) (selects what source increments the counter)
	TIM4->SMCR |= (7<<0);											// slave mode selection (External Clock mode 1) (counts up on rising edge of trigger source)
	TIM4->DIER |= (1<<0);											// enable interrupt request on update (sends interrupt when CNT == ARR)
	TIM4->SR = 0;													// clear any status flags
}

extern "C" {														// this is required for interrupts
	void TIM3_IRQHandler(void){										// interrupt handler for TIM3 (is called when TIM3 CNT == TIM3 ARR)
		TIM3-> SR = 0;												// clear flags
		TIM5 -> CR1 &= ~(1<<0);										// disable TIM5 to stop step output
		TIM3 -> CR1 &= ~(1<<0);										// disable TIM3 to reset it
		if(stepsRemaining[0] > 0){									// if there are more steps required to finish travel move
			if(stepsRemaining[0] > 0b1111111111111111){					// if the amount of steps required is larger than the auto-reload register can hold
				TIM3 -> ARR = (0b1111111111111111);							// load TIM3 with max value
				TIM3 -> CR1 |= (1<<0);										// Enable TIM3 to begin keeping track of steps
				TIM5 -> CR1 |= (1<<0);										// Enable TIM5 to begin generating steps
				stepsRemaining[0] -= 0b1111111111111111;					// subtract value loaded from steps remaining
			}else{														// if the amount of steps remaining fits into the auto-reload-register
				TIM3 -> ARR = (stepsRemaining[0]);							// load TIM3 with the remainder
				TIM3 -> CR1 |= (1<<0);										// Enable TIM3 to begin keeping track of steps
				TIM5 -> CR1 |= (1<<0);										// Enable TIM5 to begin generating steps
				stepsRemaining[0] = 0;										// clear steps remaining (because it will be completed by next call)
			}
		}else{														// if all steps were completed
			busyX = 0;												// clear the busyX flag
		}
	}
}
extern "C" {														// this is required for interrupts
	void TIM4_IRQHandler(void){										// interrupt handler for TIM4 (is called when TIM4 CNT == TIM4 ARR)
		TIM4-> SR = 0;												// clear flags
		TIM2 -> CR1 &= ~(1<<0);										// disable TIM2 to stop step output
		TIM4 -> CR1 &= ~(1<<0);										// disable TIM4 to reset it
		if(stepsRemaining[1] > 0){									// if there are more steps required to finish travel move
			if(stepsRemaining[1] > 0b1111111111111111){					// if the amount of steps required is larger than the auto-reload register can hold
				TIM4 -> ARR = (0b1111111111111111);							// load TIM4 with max value
				TIM4 -> CR1 |= (1<<0);										// Enable TIM4 to begin keeping track of steps
				TIM2 -> CR1 |= (1<<0);										// Enable TIM2 to begin generating steps
				stepsRemaining[1] -= 0b1111111111111111;					// subtract value loaded from steps remaining
			}else{														// if the amount of steps remaining fits into the auto-reload-register
				TIM4 -> ARR = (stepsRemaining[1]);							// load TIM4 with the remainder
				TIM4 -> CR1 |= (1<<0);										// Enable TIM4 to begin keeping track of steps
				TIM2 -> CR1 |= (1<<0);										// Enable TIM2 to begin generating steps
				stepsRemaining[1] = 0;										// clear steps remaining (because it will be completed by next call)
			}
		}else{														// if all steps were completed
			busyY = 0;												// clear the busyX flag
		}
	}
}


void CS(uint8_t id, uint8_t val){									// simple function to call CS of SPI devices (useful when you have lots of CS pins)
	if(val){														// if val == 1(to start call to device)
		if(id == 0){													// if id is 0
			GPIOA->ODR &= ~(1<<0);											// Set CS(PA0) Pin to LOW
		}else if(id == 1){												// if id is 1
			GPIOC->ODR &= ~(1<<0);											// Set CS(PC0) Pin to LOW
		}
	}else{															// if val == 1(to end call to device)
		if(id == 0){													// if id is 0
			GPIOA->ODR |= (1<<0);											// Set CS(PA0) Pin to HIGH
		}else if(id == 1){												// if id is 1
			GPIOC->ODR |= (1<<0);											// Set CS(PC0) Pin to HIGH
		}
	}
}

void SPIWrite40(uint8_t address, uint32_t data, uint8_t id){		// SPI function to write in a format of 40 bits (8 bits address + 32 bits data)
	uint8_t arr[5];													// create an array to store the transfer
	uint8_t* ptr = arr;												// create a pointer to that array
	arr[0] = address + 0x80; 										// add the address to the first byte of the array (plus 0b10000000) to signify a write operation
	for(uint8_t i = 0; i<4; i++){									// loop through the data, adding it to the array one byte at a time
		arr[4-i] = (data>>i*8);											// add the least significant byte of the data to the end of the array first, and then go byte by byte backwards
	}
	CS(id, 1);														// enable the chip select for the device id (sent in the id parameter) to start the transfer
	tmcCom.transmit(ptr, 5);										// use the skipper library to sent the data over SPI (pointer, data length)
	CS(id, 0);														// disable the chip select for the device, to end the transfer
}

void SPIRead40(uint8_t id, uint8_t address, uint8_t* ptr){			// SPI function to read 32 bits (40 bit structure) (for TMC2130 this needs to be done twice) (read TMC2130 datasheet for more info)
	CS(id, 1);														// enable the chip select for the device id (sent in the id parameter) to start the transfer
	tmcCom.receive(ptr, address, 5);								// use the Skipper library to read the data (this function reads the data and writes it to an array specified by the pointer parameter)
	CS(id, 0);														// disable the chip select for the device, to end the transfer
	CS(id, 1);														// enable the chip select for the device id (sent in the id parameter) to start the transfer
	tmcCom.receive(ptr, address, 5);								// use the Skipper library to read the data (this function reads the data and writes it to an array specified by the pointer parameter)
	CS(id, 0);														// disable the chip select for the device, to end the transfer
}

void driverInit(uint8_t id){										// function to initiate TMC driver (configures required registers)
	SPIWrite40(GCONF, (1<<2), id);									// write data to GCONF
	SPIWrite40(IHOLD_IRUN, HoldCurrent | (RunCurrent<<8), id);		// write current settings to IHOLD_IRUN register
	SPIWrite40(CHOPCONF, (toff<<0) | (stepResolution << 24), id);	// write CHOPCONF settings (enables the driver and sets resolution)
}

void correctBacklash(int id, uint8_t f){																				// function that is run to correct backlash
	if(id == 0){																										// X
		TIM3 -> ARR = (backlash * stepsPerMM[0]*realStepResolution);													// load distance to travel into TIM3 (this value is small, so no worry of overloading)
		TIM5 -> ARR = fabs((backlash / f)/(backlash * stepsPerMM[0]*realStepResolution)*clock.getAPB1TimerFreq());		// calculate, and load speed(freq) into TIM5
		TIM3 -> CR1 |= (1<<0);																							// enable TIM3 to begin keeping track of steps
		TIM5 -> CR1 |= (1<<0);																							// enable TIM5 to start step pulse train
		busyX = 1;																										// set busy X flag
	}else{																												// Y
		TIM4 -> ARR = (backlash * stepsPerMM[0]*realStepResolution);													// load distance to travel into TIM3 (this value is small, so no worry of overloading)
		TIM2 -> ARR = fabs((backlash / f)/(backlash * stepsPerMM[1]*realStepResolution)*clock.getAPB1TimerFreq());		// calculate, and load speed(freq) into TIM2
		TIM4 -> CR1 |= (1<<0);																							// enable TIM3 to begin keeping track of steps
		TIM2 -> CR1 |= (1<<0);																							// enable TIM5 to start step pulse train
		busyY = 1;																										// set busy Y flag
	}

	while(busyX || busyY);																								// wait for travel to finish
}

void goTo(float x, float y, uint8_t f){													// goTo function(newX, newY, speed(mm/s))
	float deltaX = x-currentX;															// calculate change in X
	float deltaY = y-currentY;															// calculate change in X
	currentX=x;																			// update current X to new X
	currentY=y;																			// update current Y to new Y
	float distance = sqrtf((deltaX*deltaX)+(deltaY*deltaY));							// calculate real distance traveled in straight line at angle
	float deltaT = distance / f;														// calculate how much time it should take in ms
	uint32_t deltaXmSteps = fabs(deltaX) * stepsPerMM[0] * realStepResolution;			// calculate how many micro-steps are required to reach new X
	uint32_t deltaYmSteps = fabs(deltaY) * stepsPerMM[1] * realStepResolution;			// calculate how many micro-steps are required to reach new Y
	uint32_t mStepsARRX = abs((deltaT/deltaXmSteps) * clock.getAPB1TimerFreq());		// calculate the speed X needs to travel to arrive on time
	uint32_t mStepsARRY = abs((deltaT/deltaYmSteps) * clock.getAPB1TimerFreq());		// calculate the speed Y needs to travel to arrive on time

	if(deltaX > 0){																		// if deltaX is larger than 0
		if(!(GPIOC->ODR & (1<<2))){															// if Dir(PC2) pin is currently LOW
			GPIOC->ODR |= (1<<2);															// set Dir(PC2) Pin to HIGH
			correctBacklash(0, f);															// insert backlash correction travel
		}
	}else if(deltaX < 0){																// if deltaX is smaller than 0
		if((GPIOC->ODR & (1<<2))){															// if Dir(PC2) pin is currently HIGH
			GPIOC->ODR &= ~(1<<2);															//Set Dir(PC2) Pin to LOW
			correctBacklash(0, f);															// insert backlash correction travel
		}
	}
	if(deltaY > 0){																		// if deltaX is larger than 0
		if(!(GPIOC->ODR & (1<<1))){															// if Dir(PC1) pin is currently LOW
			GPIOC->ODR |= (1<<1);															//Set Dir(PC1) Pin to HIGH
			correctBacklash(1, f);															// insert backlash correction travel
		}

	}else if(deltaY < 0){																// if deltaX is smaller than 0
		if((GPIOC->ODR & (1<<1))){															// if Dir(PC1) pin is currently HIGH
			GPIOC->ODR &= ~(1<<1);															// Set Dir(PC1) Pin to LOW
			correctBacklash(1, f);															// insert backlash correction travel
		}
	}

	stepsRemaining[0] = abs(deltaXmSteps);												// set how many steps are required to complete the travel move X
	stepsRemaining[1] = abs(deltaYmSteps);												// set how many steps are required to complete the travel move Y

	if(stepsRemaining[0] > 0b1111111111111111){											// if X steps required would overflow the timer
		TIM3 -> ARR = (0b1111111111111111);													// fill the timer to its max
		stepsRemaining[0] -= 0b1111111111111111;											// subtract the max from steps remaining
	}else{																				// if the timer can fit the amount required
		TIM3 -> ARR = (stepsRemaining[0]);													// fill the timer with the steps remaining
		stepsRemaining[0] = 0;																// clear steps remaining
	}
	if(stepsRemaining[1] > 0b1111111111111111){											// if Y steps required would overflow the timer
		TIM4 -> ARR = (0b1111111111111111);													// fill the timer to its max
		stepsRemaining[1] -= 0b1111111111111111;											// subtract the max from steps remaining
	}else{																				// if the timer can fit the amount required
		TIM4 -> ARR = (stepsRemaining[1]);													// fill the timer with the steps remaining
		stepsRemaining[1] = 0;																// clear steps remaining
	}

	TIM5 -> ARR = (mStepsARRX);															// fill TIM 5 with the calculated Y step frequency(speed)
	TIM2 -> ARR = (mStepsARRY);															// fill TIM 5 with the calculated Y step frequency(speed)

	TIM3 -> CR1 |= (1<<0);																// enable TIM3 to start keeping track of steps
	TIM4 -> CR1 |= (1<<0);																// enable TIM4 to start keeping track of steps

	TIM3->SR = ~(1<<0);																	// clear the TIM3 status register
	TIM4->SR = ~(1<<0);																	// clear the TIM4 status register

	if(deltaXmSteps > 0){																// if there are X steps to be completed
		TIM5 -> CR1 |= (1<<0);																// enable TIM5
		busyX = 1;																			// set busyX flag
	}
	if(deltaYmSteps > 0){																// if there are Y steps to be completed
		TIM2 -> CR1 |= (1<<0);																// enable TIM2
		busyY = 1;																			// set busyY flag
	}
}



int main(void) {									// main function (what is automatically run at boot)
	//setup
	clock.setParams(8,168,2,4,1,4,16);				// set the clock parameters
	clock.init();									// initiate the clock
	delay_ms(1000);									// wait a second for things to settle
	GPIO_init();									// initiate the GPIO to be used in this program.
	tmcCom.assignPort(SPI3);						// set up the SPI instance tmcCom (assign it to SPI3 port)
	tmcCom.setMode(3);								// set SPI mode to 3 (look up SPI modes for more info)
	tmcCom.BR = 4;									// set SPI baud rate
	tmcCom.setGPIO();								// set up the SPI GPIO pins
	tmcCom.init();									// initiate the SPI instance
	delay_ms(1000);									// wait for things to settle

	driverInit(0);									// initiate driver 0
	driverInit(1);									// initiate driver 1

	TIM2_init();									// initiate timer 2
	TIM3_init();									// initiate timer 3
	TIM4_init();									// initiate timer 4
	TIM5_init();									// initiate timer 5

	goTo(printData[0][0], 90-printData[0][1],mmPerSecond);	// goTo first coordinate
	while(busyX == 1 || busyY ==1);					// wait for travel to finish
	delay_ms(20000);								// give time to shake away first line
	for(uint32_t i = 0; i < dataArrayLength; i++){	// loop through data in array
		goTo(printData[i][0], 90-printData[i][1],mmPerSecond);	//goTo coordinate
		while(busyX == 1 || busyY ==1);					//wait for travel to finish
	}

	for(;;){
		//loop
	}
}
