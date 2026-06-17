A simple and very lightweight project to make a i2 GPIO expander, using libopencm3 and an stm32l011F3 (using less than 2kB of RAM and 8kB of flash!) 

# Instructions
 1. git clone --recurse-submodules https://github.com/Patronics/stm32l0-i2c-gpio-expander.git
 2. cd stm32l0-i2c-gpio-expander
 3. make -C libopencm3 # (Only needed once)
 4. make -C i2c-gpio

If you have an older git, or got ahead of yourself and skipped the ```--recurse-submodules```
you can fix things by running ```git submodule update --init``` (This is only needed once)

run `make -c i2c-gpio flash` to upload to the board, it's configured by default to use a CMSIS-DAP programmer, such as my [PicoProbe PCB](https://github.com/Patronics/PicoProbePCB).
