/* Timer Test Function */

/*
 * Libraries
 */
#include <atmel_start.h>
#include <stdio.h>
#include <hal_i2c_m_sync.h>
#include <hal_timer.h>
#include <utils.h>
#include <utils_assert.h>
#include <stdbool.h>

/* Global Timer Counter */
volatile uint8_t timer_c = 0;

void toggleYellowLED()
{
    if (timer_c == 100)
    {
        /* Toggle the Yellow LED Each Second */
        // Toggle the GPIO pin connected to the LED
        gpio_toggle_pin_level(USER_LED);
		printf("Turn On the User LED\n");
        timer_c = 0;
    }
	else
	{
		printf("Turn Off the User LED\n");
	}
    timer_c++;
}

int main()
{
    atmel_start_init();

    while (1)
    {
        toggleYellowLED();
    }
}
