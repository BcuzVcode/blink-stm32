#include "stm32f10x.h"

void I2C_Init(void);
void SSD1306_SendCommand(uint8_t command);
void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_SendData(uint8_t);
void SSD1306_DrawCharBitmap(char c);
void SSD1306_DrawStringBitmap(const char *str);
static void delay_ms(uint32_t);
void I2C_Init(void) {
    // Enable clock for I2C1 and GPIOB
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // Set PB6 and PB7 to Alternate Function Open-Drain (I2C)
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6); // Clear PB6
    GPIOB->CRL |= GPIO_CRL_CNF6_0 | GPIO_CRL_CNF6_1; // PB6: Output mode, Max speed 2 MHz, Open-drain
    GPIOB->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7); // Clear PB7
    GPIOB->CRL |= GPIO_CRL_CNF7_0 | GPIO_CRL_CNF7_1; // PB7: Output mode, Max speed 2 MHz, Open-drain

    // Configure I2C1
    I2C1->CR2 = 36;                  // 36 MHz clock speed
    I2C1->CCR = 180;                 // Set the clock control register (100 kHz)
    I2C1->TRISE = 37;                // Maximum rise time (37 ns)
    I2C1->CR1 |= I2C_CR1_PE;         // Enable I2C1
}

#define SSD1306_ADDRESS 0x78 // I2C address for the SSD1306

void SSD1306_SendCommand(uint8_t command) {
    // Start I2C communication
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)); // Wait for START condition

    I2C1->DR = SSD1306_ADDRESS; // Send the device address
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Wait for address transmission

    (void)I2C1->SR2; // Clear ADDR register by reading SR2

    I2C1->DR = 0x00; // Send command mode
    while (!(I2C1->SR1 & I2C_SR1_BTF)); // Wait for data transmission

    I2C1->DR = command; // Send the command
    while (!(I2C1->SR1 & I2C_SR1_BTF)); // Wait for data transmission

    I2C1->CR1 |= I2C_CR1_STOP; // Send STOP condition
}

void SSD1306_Init(void) {
    // Initialization sequence for SSD1306
    SSD1306_SendCommand(0xAE); // Display off
    SSD1306_SendCommand(0xD5); // Set display clock divide ratio
    SSD1306_SendCommand(0x80); // Suggested ratio
    SSD1306_SendCommand(0xA8); // Set multiplex
    SSD1306_SendCommand(0x3F); // 1/64 duty
    SSD1306_SendCommand(0xD3); // Set display offset
    SSD1306_SendCommand(0x00); // No offset
    SSD1306_SendCommand(0x40); // Set start line
    SSD1306_SendCommand(0xAD); // Set charge pump
    SSD1306_SendCommand(0x8B); // Enable charge pump
    SSD1306_SendCommand(0xA1); // Set segment remap
    SSD1306_SendCommand(0xC8); // Set COM output scan direction
    SSD1306_SendCommand(0xDA); // Set COM pins hardware configuration
    SSD1306_SendCommand(0x12);
    SSD1306_SendCommand(0x81); // Set contrast control
    SSD1306_SendCommand(0x7F); // Maximum contrast
    SSD1306_SendCommand(0xA4); // Disable entire display on
    SSD1306_SendCommand(0xA6); // Set normal display
    SSD1306_SendCommand(0xAF); // Display on
}

void SSD1306_Clear(void) {
    for (uint8_t i = 0; i < 8; i++) { // There are 8 pages in a 128x64 display
        SSD1306_SendCommand(0xB0 + i); // Set page address
        SSD1306_SendCommand(0x00); // Lower column address
        SSD1306_SendCommand(0x10); // Higher column address
        
        // Clear the page
        for (uint8_t j = 0; j < 128; j++) {
            SSD1306_SendCommand(0x00); // Send data (0 to clear)
        }
    }
}

void SSD1306_SendData(uint8_t data) {
    // Start I2C communication
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = SSD1306_ADDRESS;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    I2C1->DR = 0x40; // Control byte for data
    while (!(I2C1->SR1 & I2C_SR1_BTF));

    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));

    I2C1->CR1 |= I2C_CR1_STOP;
}

const uint8_t font8x8[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (Space)
    {0x00, 0x00, 0x5F, 0x5F, 0x00, 0x00, 0x00, 0x00}, // '!'
    {0x00, 0x03, 0x07, 0x00, 0x03, 0x07, 0x00, 0x00}, // '"'
    {0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F, 0x14, 0x00}, // '#'
    {0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12, 0x00, 0x00}, // '$'
    {0x46, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x62, 0x00}, // '%'
    {0x30, 0x7A, 0x4F, 0x4D, 0x7B, 0x30, 0x48, 0x00}, // '&'
    {0x00, 0x04, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00}, // '''
    {0x00, 0x1C, 0x3E, 0x63, 0x41, 0x00, 0x00, 0x00}, // '('
    {0x00, 0x41, 0x63, 0x3E, 0x1C, 0x00, 0x00, 0x00}, // ')'
    {0x22, 0x14, 0x7F, 0x7F, 0x14, 0x22, 0x00, 0x00}, // '*'
    {0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08, 0x00, 0x00}, // '+'
    {0x00, 0xA0, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}, // '.'
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // '/'
    {0x3E, 0x7F, 0x59, 0x4D, 0x7F, 0x3E, 0x00, 0x00}, // '0'
    {0x40, 0x42, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00}, // '1'
    {0x62, 0x73, 0x59, 0x49, 0x6F, 0x66, 0x00, 0x00}, // '2'
    {0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00}, // '3'
    {0x18, 0x1C, 0x16, 0x73, 0x7F, 0x70, 0x00, 0x00}, // '4'
    {0x2F, 0x6F, 0x45, 0x45, 0x7D, 0x39, 0x00, 0x00}, // '5'
    {0x3E, 0x7F, 0x49, 0x49, 0x79, 0x30, 0x00, 0x00}, // '6'
    {0x01, 0x71, 0x79, 0x0D, 0x07, 0x03, 0x00, 0x00}, // '7'
    {0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00}, // '8'
    {0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E, 0x00, 0x00}, // '9'
    {0x00, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00}, // ':'
    {0x00, 0xAC, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00}, // ';'
    {0x08, 0x1C, 0x36, 0x63, 0x41, 0x00, 0x00, 0x00}, // '<'
    {0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00}, // '='
    {0x41, 0x63, 0x36, 0x1C, 0x08, 0x00, 0x00, 0x00}, // '>'
    {0x02, 0x03, 0x59, 0x5D, 0x07, 0x06, 0x00, 0x00}, // '?'
    {0x3E, 0x7F, 0x41, 0x5D, 0x55, 0x1F, 0x1E, 0x00}, // '@'
    {0x7C, 0x7E, 0x13, 0x13, 0x7E, 0x7C, 0x00, 0x00}, // 'A'
    {0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00}, // 'B'
    {0x3E, 0x7F, 0x41, 0x41, 0x63, 0x22, 0x00, 0x00}, // 'C'
    {0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1C, 0x00, 0x00}, // 'D'
    {0x7F, 0x7F, 0x49, 0x49, 0x41, 0x41, 0x00, 0x00}, // 'E'
    {0x7F, 0x7F, 0x09, 0x09, 0x01, 0x01, 0x00, 0x00}, // 'F'
    {0x3E, 0x7F, 0x41, 0x49, 0x7B, 0x3A, 0x00, 0x00}, // 'G'
    {0x7F, 0x7F, 0x08, 0x08, 0x7F, 0x7F, 0x00, 0x00}, // 'H'
    {0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00, 0x00}, // 'I'
    {0x20, 0x60, 0x41, 0x7F, 0x3F, 0x01, 0x00, 0x00}, // 'J'
    {0x7F, 0x7F, 0x1C, 0x36, 0x63, 0x41, 0x00, 0x00}, // 'K'
    {0x7F, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00}, // 'L'
    {0x7F, 0x7E, 0x0C, 0x0C, 0x7E, 0x7F, 0x00, 0x00}, // 'M'
    {0x7F, 0x7F, 0x0E, 0x1C, 0x7F, 0x7F, 0x00, 0x00}, // 'N'
    // Continue similarly for 'O' to '~'
};

void SSD1306_DrawCharBitmap(char c) {
    if (c < 32 || c > 127) {
        c = ' '; // Replace unsupported characters with a space
    }
    
    const uint8_t *char_bitmap = font8x8[c - 32]; // Get bitmap for the character

    for (int i = 0; i < 8; i++) {
        SSD1306_SendData(char_bitmap[i]); // Send each row of the character
    }
    
    // Optionally add spacing
    SSD1306_SendData(0x00);
}

void SSD1306_DrawStringBitmap(const char *str) {
    while (*str) {
        SSD1306_DrawCharBitmap(*str++);
    }
}

uint8_t I2C_Scan(void) {
    for (uint8_t address = 1; address < 127; address++) {
        // Start I2C communication
        I2C1->CR1 |= I2C_CR1_START;
        while (!(I2C1->SR1 & I2C_SR1_SB)); // Wait for START condition

        I2C1->DR = (address << 1); // Send address with R/W bit cleared (write)
        while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Wait for address transmission

        (void)I2C1->SR2; // Clear ADDR register by reading SR2

        if (I2C1->SR1 & I2C_SR1_AF) { // Check for acknowledge failure
            // No device at this address
            I2C1->CR1 |= I2C_CR1_STOP; // Send STOP condition
            continue; // Try next address
        }

        // Device found
        I2C1->CR1 |= I2C_CR1_STOP; // Send STOP condition
        return address; // Return found address
    }
    return 0; // No device found
}

// Simple delay function
static void delay_ms(uint32_t ms) {
    // Assuming 72MHz system clock
    ms *= 12000;  // Rough approximation, may need adjustment
    while(ms--) {
        __NOP();
    }
}

int main(void){
    // Call SystemInit to initialize the system
    SystemInit();
		
		GPIOC->CRH &= ~(0xF << (13 - 8) * 4);  // Clear the bits for pin 13 (CRH because PC13 is in the high register)
		GPIOC->CRH |= (0x3 << (13 - 8) * 4);   // Set mode to output 50MHz

		I2C_Init(); // Initialize I2C
		
		if(I2C_Scan()){
					GPIOC->BRR = (1 << 13);
		}
		else{
				while(1){
						//Main Loop
						GPIOC->BSRR = (1 << 13);    // Set PC13 high
						delay_ms(200);
        
						// Turn LED OFF - Reset pin
						GPIOC->BRR = (1 << 13);     // Set PC13 low
						delay_ms(200);						
				}
		}
}
