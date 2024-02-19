/* Libraries */
#include "asf.h"
#include "stdbool.h"

/* Function Prototype */
void tc_callback_to_toggle_led(struct tc_module *const module_inst);

/* Global Variables */
struct tc_module tc_instance;
PortGroup *porta = (PortGroup *)PORT;
TcCount16 *ptc = (TcCount16 *)TC3;

/* Init Ports */
void configure_port_pin(void)
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA17, &config_port_pin);
} 

/* Init Clocks */
void enable_tc_clocks(void)
{
	struct system_gclk_chan_config gclk_chan_conf;
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBA, PM_APBCMASK_TC3);
	gclk_chan_conf.source_generator = GCLK_GENERATOR_0;
	system_gclk_chan_set_config(TC3_GCLK_ID, &gclk_chan_conf);
	system_gclk_chan_enable(TC3_GCLK_ID);
}

/* Config Timer Counter */
void config_timer_counter(void)
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size      = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation   = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.clock_prescaler   = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_16_bit.compare_capture_channel[0] = (0x0F41/2);
	tc_init(&tc_instance, TC3, &config_tc);
	tc_enable(&tc_instance);
}

/* Config Timer Counter Callback  */
void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_OVERFLOW);
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_CC_CHANNEL0);
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_CC_CHANNEL1);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL1);
}

/* Main Function */
int main(void)
{
	system_init();
	system_clock_init();
	configure_port_pin();
	enable_tc_clocks();
	config_timer_counter();
	configure_tc_callbacks();
	
	while (1)
	{
		
	}
}

/* Subroutines */
void tc_callback_to_toggle_led(struct tc_module *const module_inst)
{
	port_pin_toggle_output_level(PIN_PA17);
}
