#include "api.h"
#include "api-asm.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include <stdbool.h>

#define LED GPIOA, GPIO7

//uses pins PB6 (SCL) and PB7 (SDA) for i2c


#define BLINK_DELAY 10000
#define I2C_ADDRESS 37

volatile uint8_t inbyte = 0;
volatile bool byte_received = false;

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
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7); //set pins to alternate function (i2c1)
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO6 | GPIO7); //set pins to open drain and 50MHz speed
	gpio_set_af(GPIOB, GPIO_AF1, GPIO6 | GPIO7); //set alternate function 1: i2c
	i2c_set_7bit_addr_mode(I2C1);
	i2c_set_own_7bit_slave_address(I2C1, I2C_ADDRESS);
	I2C_OAR1(I2C1) |= I2C_OAR1_OA1EN_ENABLE;
	i2c_enable_analog_filter(I2C1);
	i2c_set_digital_filter(I2C1, 0);
	i2c_set_speed(I2C1, i2c_speed_fm_400k, 16);
	i2c_enable_stretching(I2C1);
	i2c_enable_interrupt(I2C1, I2C_CR1_ADDRIE | I2C_CR1_RXIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE); //enable interrupts. Also available: I2C_CR1_ERRIE, etc
	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_peripheral_enable(I2C1);
}

void i2c1_isr(void){
	uint32_t isr_status = I2C_ISR(I2C1);
	if(isr_status & I2C_ISR_ADDR) {
		//address match, clear flag to release clock
		I2C_ICR(I2C1) = I2C_ICR_ADDRCF;
	}
	if(isr_status & I2C_ISR_RXNE){
		//byte received, reading RXDR clears flag automatically
		inbyte = i2c_get_data(I2C1);
	}
	if (isr_status & I2C_ISR_STOPF){ //clear the stop, mark data ready
		I2C_ICR(I2C1) = I2C_ICR_STOPCF;
		byte_received = true;
	}
	
	if (isr_status & I2C_ISR_NACKF) { //clear a nack flag
    	I2C_ICR(I2C1) = I2C_ICR_NACKCF;
	}


}

int main(void) {
	clock_setup();
	gpio_setup();
	//TODOTODOTODO//////: setup NVIC interrupt handling for i2c
	//follow this example https://github.com/amitesh-singh/i2c-slave-stm32f1/blob/master/main.cpp#L49
	//nvic docuemntation: https://libopencm3.org/docs/latest/stm32l0/html/group__CM3__nvic__defines.html#gae5fdef5fd0dc9db35df8e84715fe8179
	i2c_setup();
	
	
	while(1) {
		/* wait a little bit */
		for (int i = 0; i < BLINK_DELAY; i++) {
			__asm__("nop");
		}
		if (byte_received){
			gpio_toggle(LED);
			byte_received = false;
		}
	}
	//uint32_t rev = 0xaabbccdd;
	//rev = rev_bytes(rev);
	//return my_func(rev);
}
