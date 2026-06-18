#include "api.h"
#include "api-asm.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include <stdbool.h>

//generic status LED on PB1, currently blinks when i2c communication received
#define LED_PORT GPIOB
#define LED_PIN GPIO1

//uses pins PB6 (SCL) and PB7 (SDA) for i2c

//inputs: PA4 through PA7
#define IN_PORT GPIOA
#define IN_PINS (GPIO4 | GPIO5 | GPIO6 | GPIO7)
#define IN_PORT_SHIFT 4            //shift pin register down to pack into lower bits
#define IN_MASK 0x0F               //4 input bits

//outputs: PC14 and PC15
#define OUT_PORT GPIOC
#define OUT_PINS (GPIO14 | GPIO15)
#define OUT_PORT_SHIFT 14        //shift low bits up to match pin positions
#define OUT_MASK 0x03            //2 output bits


#define BLINK_DELAY 10000
#define I2C_ADDRESS 37

volatile uint8_t inbyte = 0;   //input GPIO states, sampled continuously and sent on i2c read
volatile uint8_t outbyte = 0;  //output GPIO states received from master on i2c write
volatile bool is_read_transfer = false; //status of current ongoing i2c transfer
volatile bool new_read = false;    //flag when new read to master completes, currently unused
volatile bool new_write = false;   //flag when new write from master completes

static void clock_setup(void){

	rcc_set_sysclk_source(RCC_HSI16); //use internal 16MHz oscillator
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_I2C1);
	
}

static void gpio_setup(void){

	//generic status LED
	gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
	gpio_set(LED_PORT, LED_PIN);
	
	//expander inputs with pull-ups
	gpio_mode_setup(IN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, IN_PINS);
	
	//expander outputs
	gpio_mode_setup(OUT_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_PINS);
	gpio_clear(OUT_PORT, OUT_PINS);
	
}


//derived from examples at https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/other/i2c_stts75_sensor/i2c_stts75_sensor.c and https://github.com/libopencm3/libopencm3-examples/blob/15637e291b8ca228e35d5f657ed15f3b8958fa0c/examples/stm32/f3/stm32f3-discovery/i2c/i2c.c
//additional example of i2c slave mode: https://github.com/amitesh-singh/i2c-slave-stm32f1/blob/master/main.cpp#L49

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
	i2c_enable_interrupt(I2C1, I2C_CR1_ADDRIE | I2C_CR1_RXIE | I2C_CR1_TXIE
		| I2C_CR1_STOPIE | I2C_CR1_NACKIE); //enable interrupts. Also available: I2C_CR1_ERRIE, etc
	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_peripheral_enable(I2C1);
}

void i2c1_isr(void){
	uint32_t isr_status = I2C_ISR(I2C1);
	
	//address match
	if(isr_status & I2C_ISR_ADDR) {
		is_read_transfer = !!(isr_status & I2C_ISR_DIR_READ);
		I2C_ICR(I2C1) = I2C_ICR_ADDRCF; //clear flag to release clock stretching
	}
	
	//read from master
	if(isr_status & I2C_ISR_TXIS) {
		I2C_TXDR(I2C1) = inbyte;
	}
	
	//byte received from master
	if(isr_status & I2C_ISR_RXNE){
		//reading RXDR clears flag automatically
		outbyte = I2C_RXDR(I2C1); //equivalent to i2c_get_data(I2C1);
	}
	if (isr_status & I2C_ISR_STOPF){ //clear the stop, mark data ready
		I2C_ICR(I2C1) = I2C_ICR_STOPCF;
		if(is_read_transfer){
			new_read = true;
		} else {
			new_write = true;
		}
	}
	
	if (isr_status & I2C_ISR_NACKF) { //clear a nack flag
    	I2C_ICR(I2C1) = I2C_ICR_NACKCF;
	}


}

int main(void) {
	clock_setup();
	gpio_setup();
	i2c_setup();
	
	
	while(1) {
		//continuously sample inputs for fast response
		inbyte = (gpio_get(IN_PORT, IN_PINS) >> IN_PORT_SHIFT) & IN_MASK;
		
		if (new_write){
			new_write = false;
			uint16_t new_out = (outbyte & OUT_MASK) << OUT_PORT_SHIFT;
			gpio_port_write(OUT_PORT, (gpio_port_read(OUT_PORT) & ~OUT_PINS) | new_out);
			gpio_toggle(LED_PORT, LED_PIN);
		}
		
		if (new_read) {
			new_read = false;
			//inbyte was already sent in TXIS, no action needed here
			gpio_toggle(LED_PORT, LED_PIN);
		}
	}
}
