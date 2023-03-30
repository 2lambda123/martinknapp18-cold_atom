#include <cstdint>
#include <stdint.h>
#if defined(TARGET_STM32F4)
#include "BurstSPI.h"
 
void BurstSPI::fastWrite(int data) {
    
    SPI_TypeDef *spi = _peripheral->spi.spi.handle.Instance;
    
    // Check if data is transmitted
    while ((spi->SR & SPI_SR_TXE) == 0);
    spi->DR = data;

    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0); // wait until finished with BSY
}


void BurstSPI::fastWrite_three_byte(int data) {

    uint8_t reg_data_buf[3];
    reg_data_buf[0] = ((0xFF0000 & data) >> 16);
    reg_data_buf[1] = ((0xFF00 & data) >> 8);
    reg_data_buf[2] = (0xFF & data);

    SPI_TypeDef *spi = _peripheral->spi.spi.handle.Instance;

    for(int i = 0; i < 3; i++){
        // Check if data is transmitted
        while ((spi->SR & SPI_SR_TXE) == 0);
        spi->DR = reg_data_buf[i];
    }

    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0); // wait until finished with BSY
}


uint16_t BurstSPI::fastRead( int reg ) {

    uint16_t rtn_val = 0;
    uint16_t read_data = 0;

    SPI_TypeDef *spi = _peripheral->spi.spi.handle.Instance;

    // Check if data is transmitted
    while ((spi->SR & SPI_SR_TXE) == 0);
    spi->DR = reg;
    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0); // wait until finished with BSY

    int dummy = spi->DR;
    spi->DR = 0xFF;
    //While busy, keep checking
    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0);
    // Check RX buffer readable
    while ((spi->SR & SPI_SR_RXNE) == 0);
    read_data = spi->DR;

    spi->DR = 0xFF;
    //While busy, keep checking
    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0);
    // Check RX buffer readable
    while ((spi->SR & SPI_SR_RXNE) == 0);
    rtn_val = spi->DR;

    rtn_val = ((uint16_t)(read_data)*256) | (rtn_val);

    return rtn_val;

}


void BurstSPI::clearRX( void ) {
    //Check if the RX buffer is busy
    SPI_TypeDef *spi = _peripheral->spi.spi.handle.Instance;

    //While busy, keep checking
    while (spi->SR & SPI_SR_BSY){   
        // Check RX buffer readable
        while ((spi->SR & SPI_SR_RXNE) == 0);
        int dummy = spi->DR;
    }
}
#endif

