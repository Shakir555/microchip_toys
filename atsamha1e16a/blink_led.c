#include <asf.h>
#include <stdint.h>

// Define the base address for PORTA in specific microcontroller
// Replace with an actual base address
#define PORTA_BASE_ADDRESS 0x41004400

// Define offsets for DIR and OUT registers based on the provided
// Register Summary
#define DIR_OFFSET 0x00
#define OUT_OFFSET 0x10

// Define the bit position for PA02
#define PA02_BIT_POS 2

// Function prototypes
void configurePA02AsOutput(void);
void turnOnLED(void);
void turnOffLED(void);
void delay(void);

// Function to configure PA02 as an output
void configurePA02AsOutput(void) {
	// Set the corresponding bit in DIR register to configure PA02 as an output
	*(volatile uint32_t*)(PORTA_BASE_ADDRESS + DIR_OFFSET) |= (1 << PA02_BIT_POS);
}

// Function to turn on the LED connected to PA02
void turnOnLED(void) {
	// Set the corresponding bit in OUT register to turn on the LED
	*(volatile uint32_t*)(PORTA_BASE_ADDRESS + OUT_OFFSET) |= (1 << PA02_BIT_POS);
}

// Function to turn off the LED connected to PA02
void turnOffLED(void) {
	// Clear the corresponding bit in OUT register to turn off the LED
	*(volatile uint32_t*)(PORTA_BASE_ADDRESS + OUT_OFFSET) &= ~(1 << PA02_BIT_POS);
}

// Simple delay function (adjust as needed)
void delay(void) {
	for (volatile uint32_t i = 0; i < 1000000; i++) {
		// Adjust the loop count for the desired delay duration
	}
}

int main(void) {
	// Initialize the system clock
	system_init();
	// Initialize the board
	board_init();

	configurePA02AsOutput();

	while (1) {
		// Turn On the LED
		turnOnLED();
		delay();

		// Turn Off the LED
		turnOffLED();
		delay();
	}
}
