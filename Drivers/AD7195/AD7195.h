#ifndef AD7195_H
#define AD7195_H

#include "mbed.h"
#include "BurstSPI.h"
#include "AD7195Hex.h"


namespace drivers_AD {
namespace ad7195 {

class AD7195
{
    public:
    
    enum CmdResult
    {
        ///Failed operation
        OpFailure, 
        ///Successful operation
        Success 
    };
    
    ///@brief MAX11300 Constructor
    ///@param[in] spi_bus - reference to SPI bus for this device
    ///@param[in] cs - pin to be used for chip select
    ///@param[in] interrupt - pin to be used as interrupt input, default = NC
    ///@param[in] cnvrt - pin to be used for convert, default = NC
    AD7195(SPI & spi_bus, PinName cs);
    
    ///@brief MAX11300 Destructor
    ~AD7195();

    // ///@brief MAX11300 Constructor
    // ///@param[in] spi_bus - reference to SPI bus for this device
    // ///@param[in] cs - pin to be used for chip select
    // ///@param[in] interrupt - pin to be used as interrupt input, default = NC
    // ///@param[in] cnvrt - pin to be used for convert, default = NC
    // AD7195(BurstSPI & spi_bus, PinName cs);
    
    // ///@brief MAX11300 Destructor
    // ~AD7195();


    void init(void);
    
    ///@brief Writes given register with data
    void write_register(AD7195RegSelect_t reg, uint8_t size, uint32_t data);
    
    ///@brief Reads given register
    uint32_t read_register(AD7195RegSelect_t reg, uint8_t size);
    
    

    // Custom
    void max_speed_adc_read(uint32_t* values, size_t num_samples);
    uint32_t read_dev_id();
    void reset();

    //
    
    private:
    
    // BurstSPI & m_spi_bus;
    SPI & m_spi_bus;
    DigitalOut m_cs;
    volatile uint8_t m_write_done;

    void spi_write_cb(int event);
    void spi_read_cb(int event);
};

} // namespace drivers
} // namespace ad7195

#endif /* AD7195_H */