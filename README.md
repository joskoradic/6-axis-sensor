This is driver for 6 axis sensor written in C.
Project uses underlying SPI HAL driver from STM32, 
however you can replace it with your own SPI driver , according your MCU.
To do so , replace:
spi_write_reg()
spi_read_reg()

functions with your own
