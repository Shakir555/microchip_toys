/*
 * Libraries
 */
#include <atmel_start.h>
#include <stdio.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	/* Initialize I2C Peripheral */
	i2c_m_sync_enable(&I2C_0);
	
	/* I2C Device address */
	for (uint8_t address = 1; address <= 127; address++)
	{
		struct io_descriptor *io;
		int8_t result = i2c_m_sync_get_io_descriptor(&I2C_0, &io);
		
		if (result == 0)
		{
			/* Try to read 1 byte from the current address */
			if (io_write(io, &address, 1) == 0)
			{
				/* If successful, an ACK was received, meaning a device is at this address */
				printf("I2C Device Detected at address 0x%02X\n", address);
			}
			else
			{
				/* If unsuccessful, print that the device is not found */
				printf("I2C Device not found at address 0x%02X\n", address);
			}
		}
	} 
}
