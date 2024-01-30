/*
* Libraries
*/
#include <atmel_start.h>

/* MPU6050 I2C Address */
#define MPU6050_ADD        0x68
#define MPU6050_WHO_AM_I   0x75
#define MPU6050_PWR_MGMT_1 0x6B

int main(void)
{
	/* I2C Descriptor */
	struct io_descriptor *I2C_0_io;
	
	/* I2C Command Data */
	uint8_t command_data[1];
	
	/* I2C Initialization */
	i2c_m_sync_get_io_descriptor(&I2C_0, &I2C_0_io);
	i2c_m_sync_enable(&I2C_0);
	i2c_m_sync_set_slaveaddr(&I2C_0, MPU6050_ADD, I2C_M_SEVEN);
	
	/* Read WHO_AM_I Register */
	command_data[0] = MPU6050_WHO_AM_I;
	i2c_m_sync_cmd_write(&I2C_0, MPU6050_WHO_AM_I, command_data, 1);
	
	/* Delay to allow the sensor to respond */
	delay_ms(10);
	i2c_m_sync_cmd_read(&I2C_0, MPU6050_WHO_AM_I, command_data, 1);
	
	/* Check if the WHO_AM_I Value is correct (should be 0x68 for MPU6050) */
	if (command_data[0] == 0x60)
	{
		/* MPU6050 is detected, perform additional operations if needed */
		/* Disable Sleep Mode by writing 0 to PWR_MGMT_1 register */
		command_data[0] = MPU6050_PWR_MGMT_1;
		i2c_m_sync_cmd_write(&I2C_0, MPU6050_PWR_MGMT_1, command_data, 1);
		delay_ms(10);
		printf("MPU6050 is found. Turn on LED\n");
	}
	else
	{
		/* Error Handling Code here: Turn on LED */
		printf("MPU6050 not found. Turn off LED\n");
	}
	while(1)
	{
		/* Run While loop here */
	}
}
