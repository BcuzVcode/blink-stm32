#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f10x.h"

// Port definitions (unchanged)
#define PortA          0           
#define PortB          1           
#define PortC          2  
#define PortD          3 
#define PortE          4 
#define PortF          5 
#define PortG          6 

// GPIO modes for STM32F1
#define IN            0    // Input mode
#define OUT_10MHz    1    // Output mode, max speed 10 MHz
#define OUT_2MHz     2    // Output mode, max speed 2 MHz
#define OUT_50MHz    3    // Output mode, max speed 50 MHz

// Input modes
#define ANALOG       0x0   // Analog mode
#define FLOATING     0x4   // Floating input
#define INPUT_PU_PD  0x8   // Input with pull-up/pull-down

// Output modes
#define GP_PP        0x0   // General purpose push-pull
#define GP_OD        0x4   // General purpose open-drain
#define AF_PP        0x8   // Alternate function push-pull
#define AF_OD        0xC   // Alternate function open-drain

#define HIGH         1
#define LOW          0

#define FALL         0
#define RISE         1

// Pin enums (unchanged)
enum portA{PA0=0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15}; 
enum portB{PB0=16,PB1,PB2,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15}; 
enum portC{PC0=32,PC1,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15}; 
enum portD{PD0=48,PD1,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PD10,PD11,PD12,PD13,PD14,PD15}; 
enum portE{PE0=64,PE1,PE2,PE3,PE4,PE5,PE6,PE7,PE8,PE9,PE10,PE11,PE12,PE13,PE14,PE15}; 
enum portF{PF0=80,PF1,PF2,PF3,PF4,PF5,PF6,PF7,PF8,PF9,PF10,PF11,PF12,PF13,PF14,PF15}; 
enum portG{PG0=96,PG1,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PG10,PG11,PG12,PG13,PG14,PG15}; 

typedef struct pinMaster {
    char port;
    char pin;
} Pin;

// Function prototypes
Pin initPin(char port, char pin);
Pin Pin_OutputSet(char port, char pin);
Pin Pin_InputSet(char port, char pin);
Pin Pin_AFSet(char port, char pin);
void Timer_Pin_set(char port, char pin, char timer);
void Serial_Pin_set(char port, char pin, char uart);
void ADC_Pin_set(char port, char pin);
void Pin_W(Pin pin, char state);
char Pin_R(Pin pin);
void Pin_toggle(Pin pin);

// Single element pin control
Pin PinOutputSet(char PortPin);
Pin PinInputSet(char PortPin);
Pin PinAFSet(char PortPin);
void PinW(char PortPin, char state);
char PinR(char PortPin);
void PinToggle(char PortPin);
void PinADCSet(char PortPin);

// GPIO IRQ functions
void PinIRQFE(char PortPin);
void PinIRQRE(char PortPin);
void PinIRQFRE(char PortPin);
void PinIrqStopFE(char PortPin);
void PinIrqStopRE(char PortPin);

// Peripheral pin setup functions
void ADCPinset(char PortPin);
void SPIPinset(char PortPin, char spi);
void I2CPinset(char PortPin);
void USARTPinset(char PortPin, char usart);

#endif /* __GPIO_H */