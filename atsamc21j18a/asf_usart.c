#include <asf.h>
#include <string.h>

#define MAX_TX_BUFFER_LENGTH 5
volatile uint8_t tx_buffer[MAX_TX_BUFFER_LENGTH] = {'H', 'e', 'l', 'l', 'o'};
static struct usart_module usart_instance;

void config_usart(void);
void send_data(const uint8_t *data, size_t length);

int main(void)
{
	system_init();
	config_usart();
	system_interrupt_enable_global();

	while (1)
	{
		// Example: Send the content of tx_buffer every 1 second
		send_data(tx_buffer, sizeof(tx_buffer));
		delay_ms(1000);
	}
}

void config_usart(void)
{
	struct usart_config usart;
	usart_get_config_defaults(&usart);
	usart.baudrate = 115200;
	usart.mux_setting = USART_RX_3_TX_0_RTS_2_CTS_3;
	usart.pinmux_pad0 = PINMUX_UNUSED;
	usart.pinmux_pad1 = PINMUX_UNUSED;
	usart.pinmux_pad2 = PINMUX_PA14D_SERCOM4_PAD2;
	usart.pinmux_pad3 = PINMUX_PA15D_SERCOM4_PAD3;

	while (usart_init(&usart_instance, SERCOM4, &usart) != STATUS_OK)
	{
		printf("USART Init failed");
	}

	usart_enable(&usart_instance);
}

void send_data(const uint8_t *data, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		while (!usart_write_wait(&usart_instance, data[i]))
		{
			// Wait until data can be written
		}
	}
	// Send newline character to move to the next line (optional)
	usart_write_wait(&usart_instance, '\n');
}
