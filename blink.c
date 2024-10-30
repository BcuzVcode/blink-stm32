#include "stm32f10x.h"

void delay_ms(uint32_t ms){

	ms *= 12000;
	while(ms--){
		__NOP();
	}
	
}

int main(void){

	GPIOC->CRH &= ~(0xF << (13-8)*4);
	GPIOC->CRH |= (0x3 << (13-8)*4);

	while(1){
		GPIOC->ODR ^= (1<<13);
		delay_ms(500);
	}
}