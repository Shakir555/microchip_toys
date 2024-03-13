/* Libraries */
#include <asf.h>

/* USART Definition */
#define USART_BAUD_RATE    9600
#define USART_SAMPLE_NUM   16
#define USART_BAUD_SHIFT   32

/* USART Variables */
uint8_t usart_rx_data;
volatile bool usart_rx_started = false;

/* Function Prototype */
void usart_clock_init(void);
void usart_pin_init(void);
void usart_initialize(void);
void usart_send_string(const char *str_buf);
void sercom4_handler(void);

/* Calculate Baud Value */
uint16_t cal_baud_val(const uint32_t baudrate, const uint32_t peripheral_clock,
uint8_t sample_num);

/* USART Handler */
void sercom4_handler(void)
{
	if (SERCOM4->USART.INTFLAG.bit.RXS)
	{
		usart_rx_started = true;
	}
	if (SERCOM4->USART.INTFLAG.bit.RXC)
	{
		usart_rx_data = SERCOM4->USART.DATA.reg;
		usart_rx_started = false;
	}
}

/* Set USART Pin Peripheral Function */
static inline void pin_set_peripheral_function(uint32_t pinmux)
{
	uint8_t port = (uint8_t)((pinmux >> 16)/32);
	PORT->Group[port].PINCFG[((pinmux >> 16) - (port*32))].bit.PMUXEN = 1;
	PORT->Group[port].PMUX[((pinmux >> 16) - (port*32))/2].reg &= ~(0xF << (4 * ((pinmux >>
	16) & 0x01u)));
	PORT->Group[port].PMUX[((pinmux >> 16) - (port*32))/2].reg |= (uint8_t)((pinmux &
	0x0000FFFF) << (4 * ((pinmux >> 16) & 0x01u)));
}

/* Internal Calculate 64 bit division */
static uint64_t long_division(uint64_t n, uint64_t d)
{
	int32_t i;
	uint64_t q = 0;
	uint64_t r = 0;
	uint64_t bit_shift;
	
	for (i = 63; i >= 0; i--)
	{
		bit_shift = (uint64_t)1 << i;
		
		r = r << 1;
		
		if (n & bit_shift)
		{
			r |= 0x01;
		}
		
		if (r >= d)
		{
			r = r - d;
			q |= bit_shift;
		}
	}
	return q;
}

/* Internal Calculate Asynchronous Baudrate Value (USART) */
uint16_t cal_baud_val(const uint32_t baudrate,
const uint32_t peripheral_clock,
uint8_t sample_num)
{
	/* Temporary Variables */
	uint64_t baud_temp_val;
	uint64_t baud_ratio = 0;
	uint64_t baud_scale = 0;
	uint64_t baud_calculated = 0;
	
	/* Calculate Baud Value */
	baud_temp_val   = ((sample_num * (uint64_t)baudrate) << USART_BAUD_SHIFT);
	baud_ratio      = long_division(baud_temp_val, peripheral_clock);
	baud_scale      = ((uint64_t)1 << USART_BAUD_SHIFT) - baud_ratio;
	baud_calculated = (65536 * baud_scale) >> USART_BAUD_SHIFT;
	
	return baud_calculated;
}

/* USART (SERCOM4) bus and generic clock initialization */
void usart_clock_init(void)
{
	struct system_gclk_chan_config gclk_chan_conf;
	uint32_t gclk_index = SERCOM4_GCLK_ID_CORE;
	/* Turn on Module in PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_SERCOM4);
	/* Turn on Generic Clock for USART */
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	/* Default is Generator 0. Otherwise need to config like below */
	system_gclk_chan_set_config(gclk_index, &gclk_chan_conf);
	system_gclk_chan_enable(gclk_index);
}

/* USART (SERCOM4) Pin Initialization */
void usart_pin_init(void)
{
	pin_set_peripheral_function(PINMUX_PA14D_SERCOM4_PAD2);
	pin_set_peripheral_function(PINMUX_PA15D_SERCOM4_PAD3);
}

/* USART (SERCOM4) Initialization */
void usart_initialize(void)
{
	uint16_t baud_value;
	baud_value = cal_baud_val(USART_BAUD_RATE,
	system_gclk_chan_get_hz(SERCOM4_GCLK_ID_CORE),
	USART_SAMPLE_NUM);
	
	/* By setting the DORD bit LSB is transmitted first */
	/* Setting the RXPO bit as 3 corresponding SERCOM PAD[3] will be used for data reception */
	/* PAD[2] will be used as TxD pin by setting TXPO bit as 1 */
	/* 16x over-sampling is selected by setting the SAMPR bit as 0 */
	/* USART Clock Mode is selected as USART with internal clock by setting MODE bit into 1 */
	SERCOM4->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD				|
	SERCOM_USART_CTRLA_RXPO(0x03)		|
	SERCOM_USART_CTRLA_TXPO(0x01)		|
	SERCOM_USART_CTRLA_SAMPR(0x0)		|
	SERCOM_USART_CTRLA_MODE_USART_INT_CLK;
	/* Baud Register Value corresponds to the device communication baud rate */
	SERCOM4->USART.BAUD.reg = baud_value;
	/* 8-bits size is selected as character size by setting the bit CHSIZE as 0 */
	/* TXEN bit and RXEN bits are set to enable the Transmitter and Receiver */
	SERCOM4->USART.CTRLB.reg = SERCOM_USART_CTRLB_CHSIZE(0x0)		|
	SERCOM_USART_CTRLB_TXEN				|
	SERCOM_USART_CTRLB_RXEN				|
	SERCOM_USART_CTRLB_SFDE;
	
	/* Synchronization Busy */
	while (SERCOM4->USART.SYNCBUSY.bit.CTRLB);
	/* SERCOM3 Handler Enables */
	system_interrupt_enable(SERCOM4_IRQn);
	/* Receive Complete Interrupt, Receive Start Interrupt set */
	SERCOM4->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_RXC | SERCOM_USART_INTFLAG_RXS;
	/* SERCOM3 Peripheral Enabled */
	SERCOM4->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	/* Synchronization Busy */
	while (SERCOM4->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_ENABLE);
}

/* Function used to print the character in the terminal */
void usart_send_string(const char *str_buf)
{
	while (*str_buf != '\0')
	{
		while(!SERCOM4->USART.INTFLAG.bit.DRE);
		SERCOM4->USART.DATA.reg = *str_buf;
		str_buf++;
	}
}

/* Main Function */
int main(void)
{
	system_init();
	usart_clock_init();
	usart_pin_init();
	usart_initialize();
	sercom4_handler();
	
	usart_send_string("Hello World\r\n");
}
