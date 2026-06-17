#include "api.h"
#include "api-asm.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

#define LED GPIOA, GPIO7
#define BLINK_DELAY 100000
#define I2C_ADDRESS 0x25


static void clock_setup(void){

	rcc_set_sysclk_source(RCC_HSI16); //use internal 16MHz oscillator
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_I2C1);
	
}

static void gpio_setup(void){
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
	gpio_set(LED);
}


//derived from examples at https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/other/i2c_stts75_sensor/i2c_stts75_sensor.c and https://github.com/libopencm3/libopencm3-examples/blob/15637e291b8ca228e35d5f657ed15f3b8958fa0c/examples/stm32/f3/stm32f3-discovery/i2c/i2c.c
static void i2c_setup(void)
{
	//rcc_periph_reset_pulse(RST_I2C1);
	i2c_peripheral_disable(I2C1); //disable i2c while setting up
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO7); //set pins to output
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO6 | GPIO7); //set pins to open drain and 50MHz speed
	gpio_set_af(GPIOB, GPIO_AF1, GPIO6 | GPIO7); //set alternate function 1: i2c
	i2c_set_7bit_addr_mode(I2C1);
	i2c_set_own_7bit_slave_address(I2C1, I2C_ADDRESS);
	i2c_enable_analog_filter(I2C1);
	i2c_set_digital_filter(I2C1, 0);
	i2c_set_speed(I2C1, i2c_speed_fm_400k, 16);
	i2c_disable_stretching(I2C1);
	i2c_peripheral_enable(I2C1);
	//TODO: setup slave based on https://github.com/amitesh-singh/i2c-slave-stm32f1/blob/9dcedf2cb559553bc27aeb1b3fc2526735ea6690/main.cpp#L49
}


int main(void) {
	clock_setup();
	gpio_setup();
	i2c_setup();
	
	
	while(1) {
		/* wait a little bit */
		for (int i = 0; i < BLINK_DELAY; i++) {
			__asm__("nop");
		}
		gpio_toggle(LED);
	}
	//uint32_t rev = 0xaabbccdd;
	//rev = rev_bytes(rev);
	//return my_func(rev);
}
