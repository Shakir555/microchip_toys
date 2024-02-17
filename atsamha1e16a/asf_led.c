/* Blink project on SAMD20 using ASF.
*  Justin Owen
*
*
*/

#include "asf.h"
#include "stdbool.h"

/************************************************************************/
/* Function Prototypes                                                  */
/************************************************************************/

void tc_callback_to_toggle_led(struct tc_module *const module_inst);

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/

struct tc_module tc_instance;
PortGroup *porta = (PortGroup *)PORT;
TcCount16 *ptc = (TcCount16 *)TC3;



/* 
	!! Magic ASF code
	Enable the Interface clock and the gerneric clock for
	Timer Counter 3

*/
/************************************************************************/
/* INIT PORTS                                                           */
/************************************************************************/

void configure_port_pins(void)
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA17, &config_port_pin);
}

/************************************************************************/
/* INIT Clocks                                                          */
/************************************************************************/

void enable_tc_clocks(void)
{
	struct system_gclk_chan_config gclk_chan_conf;
	
	/* Turn on TC module in PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_TC3);

	/* Set up the GCLK for the module */
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	
	//Setup generic clock 0 (also the clock for MCU (running at 8 Mhz) as source for the timer clock)
	gclk_chan_conf.source_generator = GCLK_GENERATOR_0;
	system_gclk_chan_set_config(TC3_GCLK_ID, &gclk_chan_conf);
	
	//Enable the generic clock for the Timer/ Counter block
	system_gclk_chan_enable(TC3_GCLK_ID);
}


void config_timer_counter(void)
{
	/* configure CNTRA*/
	// disable the TC to allow writes to CTRLA
	// then zero out rest of fields
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	
	/* Configure CTRLA reg to do the following:
	 - Reload or reset the counter on next prescaler clock
	 - Waveform generation function using Match frequency
	 - Set Prescaler Value to 1024
	*/
	config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	
	// Write the proper match value into CC0
	// found with OUT_Freq = (Fclk / (N*(Top + 1))
	// I am using an output of 2Hz 
	config_tc.counter_16_bit.compare_capture_channel[0] = (0x0F41/2);
	
	// enable counter
	tc_init(&tc_instance, TC3, &config_tc);
	tc_enable(&tc_instance);
}

void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_OVERFLOW);
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_CC_CHANNEL0);
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_CC_CHANNEL1);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL1);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

int main (void)
{
	// Configure board
	system_init();
	system_clock_init();
	configure_port_pins();
	
	// Configure TC
	enable_tc_clocks();
	config_timer_counter();
	configure_tc_callbacks();
	
	while (1)
	{
		// Toggle LED with TC OVF
	}
}

/************************************************************************/
/* Subroutines                                                          */
/************************************************************************/

void tc_callback_to_toggle_led(struct tc_module *const module_inst)
{
	port_pin_toggle_output_level(PIN_PA17);
}


