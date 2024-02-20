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
void usart_read_callback(struct usart_module *const usart_module);
void usart_write_callback(struct usart_module *const usart_module);

#endif // USART_CONFIG_H
