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

    SPI_TypeDef *spi = _peripheral->spi.spi.handle.Instance;

    for(int i = 0; i < 3; i++){
        // Check if data is transmitted
        while ((spi->SR & SPI_SR_TXE) == 0);
        spi->DR = 0x55;
    }

    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0); // wait until finished with BSY
}


int BurstSPI::fastRead( void ) {

    int rtn_val = 0;

    SPI_TypeDef *spi = _peripheral->spi.spi.handle.Instance;

    while ((spi->SR & SPI_SR_TXE) == 0);
    spi->DR = 0xFF;

    while ((spi->SR & SPI_SR_RXNE) == 0); // wait until BSY
    rtn_val = spi->DR;

    while ((spi->SR & SPI_SR_BSY) == 0); // wait until BSY
    while ((spi->SR & SPI_SR_BSY) != 0); // wait until finished with BSY

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

