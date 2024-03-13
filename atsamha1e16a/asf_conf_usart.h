// usart_config.h
#ifndef USART_CONFIG_H
#define USART_CONFIG_H

#include "asf.h"
#include "stdbool.h"
#include "stdio_serial.h"

/* Global Variables */
struct usart_module usart_instance;
#define MAX_RX_BUFFER_LENGTH  5
volatile uint8_t rx_buffer [MAX_RX_BUFFER_LENGTH];
volatile unsigned int sys_timer = 0;

/* Function Prototypes */
void enable_tc_clocks(void);
void configure_usart(void);
void configure_usart_callbacks(void);
void usart_read_callback(struct usart_module *const usart_module);
void usart_write_callback(struct usart_module *const usart_module);

/* Enable tc clock */
void enable_tc_clocks(void)
{
	struct system_gclk_chan_config gclk_chan_conf;
	/* Turn on TC Module in PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_TC3);
	/* Set up the GCLK for the module */
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	/* Setup generic clock 0 (also the clock for MCU (running at 8 Mhz) */
	/* As source for the timer */
	gclk_chan_conf.source_generator = GCLK_GENERATOR_0;
	system_gclk_chan_set_config(TC3_GCLK_ID, &gclk_chan_conf);
	/* Enable the generic clock for the Timer / Counter Block */
	system_gclk_chan_enable(TC3_GCLK_ID);
}

/* Init USART */
void configure_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
	config_usart.pinmux_pad0 = PINMUX_UNUSED;
	config_usart.pinmux_pad1 = PINMUX_UNUSED;
	config_usart.pinmux_pad2 = PINMUX_PA14D_SERCOM4_PAD2;
	config_usart.pinmux_pad3 = PINMUX_PA15D_SERCOM4_PAD3;
	while (usart_init(&usart_instance,
	SERCOM4, &config_usart) != STATUS_OK)
	{
		/* Running USART */
	}
	usart_enable(&usart_instance);
}

/* Configure USART Callback */
void configure_usart_callbacks(void)
{
	usart_register_callback(&usart_instance,
	usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_instance,
	usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
}

void usart_read_callback(struct usart_module *const usart_module)
{
	usart_write_buffer_job(&usart_instance,
	(uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
}

void usart_write_callback(struct usart_module *const usart_module)
{
	port_pin_toggle_output_level(PIN_PA17);
}

void SysTick_Handler(void)
{
	sys_timer++;
}

#endif // USART_CONFIG_H
