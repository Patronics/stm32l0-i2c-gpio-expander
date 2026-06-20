A simple and very lightweight i2c slave GPIO expander, using libopencm3 and an stm32l011F3 (using only 16 bytes of RAM and 1.5kB of flash!) 

Configured for 4 inputs (PA4 through PA7), 2 outputs (PC14 and PC15), and an optional status led on PB1 that toggles each time a read or write completes.

Use pins PB6 and PB7 for i2c.

This lightweight library is perfect for use with the STM32L011F3, such as with my [STM32DIP20 breakout board](https://github.com/Patronics/STM32DIP20-breakout).
Any other STM32L0 series chip should also be compatible with this program without any modifications.

## Instructions
 1. `git clone --recurse-submodules https://github.com/Patronics/stm32l0-i2c-gpio-expander.git`
 2. `cd stm32l0-i2c-gpio-expander`
 3. `make -C libopencm3` # (Only needed once)
 4. `make -C i2c-gpio`
 5. run `make -c i2c-gpio flash` to upload to the board, it's configured by default to use a CMSIS-DAP programmer, such as my [PicoProbe PCB](https://github.com/Patronics/PicoProbePCB).



If you skipped the ```--recurse-submodules``` instruction in step 1,
you can fix it by running ```git submodule update --init``` (This is only needed once)

