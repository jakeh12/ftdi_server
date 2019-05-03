#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>

int spi_open();
int spi_shift(uint8_t target, uint16_t length, uint8_t* tx_data, uint8_t* rx_data);
int spi_close();

#endif /* MODULE_H */