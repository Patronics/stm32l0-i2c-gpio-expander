A simple and very lightweight i2c slave GPIO expander, using libopencm3 and an stm32l011F3 (using less than 100 bytes of RAM and 2kB of flash!) 

Configured for 4 inputs (PA4 through PA7), 2 outputs (PC14 and PC15), and an optional status led on PB1 that toggles each time a read or write completes.

Use pins PB6 and PB7 for i2c.

# Instructions
 1. `git clone --recurse-submodules https://github.com/Patronics/stm32l0-i2c-gpio-expander.git`
 2. `cd stm32l0-i2c-gpio-expander`
 3. `make -C libopencm3` # (Only needed once)
 4. `make -C i2c-gpio`
 5. run `make -c i2c-gpio flash` to upload to the board, it's configured by default to use a CMSIS-DAP programmer, such as my [PicoProbe PCB](https://github.com/Patronics/PicoProbePCB).





If you have an older git, or got ahead of yourself and skipped the ```--recurse-submodules```
you can fix things by running ```git submodule update --init``` (This is only needed once)

