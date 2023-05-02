/* STM32 Skipper Template
 * This file is intended to be copied to create new projects off this template,
 * Do not write your program in this template file, first copy, paste, and rename it, Then create your project.
 * Do not build, or upload this template file.
 * created by: Jack Herron
*/

/*
 * date: 			April 28 2023
 * Project name: 	Addressable LED Demo(WS2812B)
 * Created by:	 	Jack Herron
 *
 * description: 	this program runs a demo for addressable LED strip, specifically WS2812B LEDS
 *
 * using DMA(direct memory access) and PWM(pulse width modulation) allows the CPU to be completely free to do other things, and the DMA+PWM
 * will take care of all the timing/updating of the LEDS. The DMA works by facilitating a data transfer from a memory location, to a destination
 * upon a DMA request. In this example we will use a DMA request from Timer 2, to transfer data from the pixelData array to the CCR1 register of timer 2.
 * allowing us to send precisely timed pulses(0.4us and 0.8us) to the WS2812B LEDS
*/

#include <stdint.h>										// include standard c functions
#include <stm32f4xx.h>									// include MCU specific definitions
#include <Skipper.h>									// include skipper library

const uint32_t numOfPixels = 256;						// the number of LEDs(pixels) on your strip.
const uint8_t blankTime = 100;							// the number of blanks(0's) inserted after the entire transfer, in order to create a >50us pause to indicate the end of the transfer.

uint32_t pixelData[(numOfPixels*24)+blankTime];			// this array is where the raw pixel data will be stored (it will be filled with 19's and 39's representing 0's, and 1's).

void Display_init(){																//this function fills the array with the value 19(19 clock cycles = 0.4us pulse)  to represent 0's to the pixels.
	for(uint32_t i = 0; i<((sizeof(pixelData)/4)-100); i++){
		pixelData[i] = 19;
	}
}

void updatePixel(uint8_t R,uint8_t G,uint8_t B, uint8_t position){					// this function updates the specified pixel with a new color.
	uint8_t color[3] = {G,R,B};														// correct the data from RGB to GRB
	for(int c = 0; c<3; c++){														// run once for each color
		for(int i = 0; i<8; i++){													// run once for each color-bit
			pixelData[(position*24)+(8*c)+i] = ((((color[c]>>(7-i)) & 1)+1)*20)-1;	// this takes the color byte, and replaces the 1's with 39, and the 0's with 19. and enters those values into the pixelData array.
		}
	}
}

void GPIO_init(void){
	RCC->AHB1ENR |= (1<<3); 		// enable GPIOD clock
	RCC->AHB1ENR |= (1<<0);			// enable GPIOA clock

	//Enable on board LED
	GPIOD->MODER |= (1<<8); 		// set PD4 to output
	GPIOD->ODR |= (1<<4);			// set PD4 HIGH

	//Enable PA0 as PWM output
	GPIOA->MODER |= (2<<0);			// set mode to alternate function mode
	GPIOA->AFR[0] |= (1<<0);		// set alternate function as timer2 out
	GPIOA->OSPEEDR |= (3<<0);		// set speed to fast
}

void DMA_init(void){
	RCC->AHB1ENR |= (1<<21);
	DMA1_Stream5->CR &= ~(1<<0);						// make sure the DMA is disabled for setup.
	DMA1_Stream5->CR |=  (3<<25);						// select channel 3 as the stream 5 DMA location, This is because Tim2 is DMA1, stream 5, channel 3.
	DMA1_Stream5->CR |=  (2<<13);						// set the data size to word(32 bits) because this is the size of the Tim2 CCR1 register.
	DMA1_Stream5->CR |=  (2<<11);						// set the data size to word(32 bits) because this is the size of the Tim2 CCR1 register.
	DMA1_Stream5->CR |=  (1<<10);						// enable data incrementing, this will cause the DMA to read the next pixel data after each request.
	DMA1_Stream5->CR &=  ~(1<<9);						// make the peripheral address fixed, so it continues to write to the same address.
	DMA1_Stream5->CR |=  (1<<6);						// turn on memory to peripheral mode
	DMA1_Stream5->CR |=  (1<<8);						// turn on DMA circular mode, this will cause the DMA to loop back to the start address every time it reaches the end, this will cause the LEDS to update at max frequency
	DMA1_Stream5->NDTR = (numOfPixels*24) + blankTime;	// tell the DMA how much data it will loop through, (24 bits per LED, + blank time(zero bits))
	DMA1_Stream5->PAR =  (uint32_t)&TIM2->CCR1;			// set the destination address to the Timer 2 CCR1 register
	DMA1_Stream5->M0AR = (uint32_t)&pixelData;			// set the origin address to the start of the pixelData array
	DMA1_Stream5->CR |= (1<<0); 						// enable the DMA stream
}

void Tim2_init(void){				// initiate Timer 2 as PWM @800Khz
	RCC->APB1ENR |= (1<<0);			// enable the TIM2 peripheral
	TIM2->CCMR1 |= (6<<4);			// set Timer 2 mode to PWM
	TIM2->CCMR1 |= (1<<3);			// turn on output compare preload, this makes it so the DMA request value takes effect at the start of each PWM cycle
	TIM2->CCER |= (1<<0);			// enable the CC1 output to the timer pin.
	TIM2->CCR1 = 0;					// set the initial PWM duty cycle to 0.
	TIM2->ARR = (60)-1;				// set the auto reload value to 60 (48Mhz/60 = 800Khz = 1.25us PWM period).
	TIM2->DIER |= (1<<9);			// turn on capture/compare DMA request, this will cause the timer to request the next bit of data every cycle.
	TIM2->CR1 |= (1<<0);			// turn on the timer
}

int main(void) {
	//clock setup:
	//the system clock will operate at 96Mhz
	//the Tim2 peripheral(APB1) will operate at 48Mhz
	//
	clock.setParams(8,96,2,8,1,4,2);	// set the clock parameters (M,N,P,Q,AHB,APB1,APB2). It is recommended to use cubemx to find these values.
	clock.init();						// initiate the clock
	GPIO_init();						// initiate the GPIO to be used in this program.
	Display_init();						// initiate the pixelData array, this will fill it with all 19's(this will cause a 19 clock cycle pulse indicating a zero on the led)
	DMA_init();							// initiate the DMA
	Tim2_init();						// initiate Timer 2
	delay_ms(1000);						// 1 second pause


	for(;;){
		for(uint32_t i = 0; i<256; i++){
				updatePixel(i,0,0,i);
				delay_ms(1);
		}
		for(uint32_t i = 0; i<256; i++){
				updatePixel(0,i,0,i);
				delay_ms(1);
		}
		for(uint32_t i = 0; i<256; i++){
				updatePixel(0,0,i,i);
				delay_ms(1);
		}


	}
}

