/* STM32 Skipper Template

*This file is intended to be copied to create new projects off this template,
*Do not write your program in this template file, first copy and rename it, Then create your project.
*
*created by: Jack Herron
*/

/*
 * date [date]
 * Project name - [your project name]
 * Created by: [your name]
 *
 * description[description]
 */


#include <stdint.h>					// include standard c functions
#include <stm32f4xx.h>				// include MCU specific definitions
#include <Skipper.h>				// include skipper library

int main(void) {
	//setup
	clock_init(8,168,2,4,1,4,2);	//initiate the clock (M,N,P,Q,AHB,APB1,APB2). It is recommended to use cubemx to find these values.
	GPIO_init();					//initiate the GPIO to be used in this program.


	for(;;){
		//infinite loop

	}
}

void GPIO_init(void){
	RCC->AHB1ENR |= (1<<3); 		// enable GPIOD clock

	//Enable on board LED
	GPIOD->MODER |= (1<<8); 		// set PD4 to output
	GPIOD->ODR |= (1<<4);			// set PD4 HIGH
}
