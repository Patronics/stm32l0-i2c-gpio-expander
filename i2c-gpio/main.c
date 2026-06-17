#include "api.h"
#include "api-asm.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

#define LED GPIOA, GPIO7
#define LITTLE_BIT 100000

int main(void) {
	/* add your own code */
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
	gpio_set(LED);
	
	while(1) {
		/* wait a little bit */
		for (int i = 0; i < LITTLE_BIT; i++) {
			__asm__("nop");
		}
		gpio_toggle(LED);
	}
	//uint32_t rev = 0xaabbccdd;
	//rev = rev_bytes(rev);
	//return my_func(rev);
}
