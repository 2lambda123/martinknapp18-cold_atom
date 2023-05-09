#ifndef AD5781_H
#define AD5781_H

#include "mbed.h"
#include "BurstSPI.h"
#include "AD5781Hex.h"


namespace drivers_AD {
namespace ad5781 {

class AD5781
{
    public:
    
    enum CmdResult
    {
        ///Failed operation
        OpFailure, 
        ///Successful operation
        Success 
    };
    
    ///@brief AD5781 Constructor
    ///@param[in] spi_bus - reference to SPI bus for this device
    ///@param[in] cs - pin to be used for chip select
    ///@param[in] interrupt - pin to be used as interrupt input, default = NC
    ///@param[in] cnvrt - pin to be used for convert, default = NC
    AD5781(SPI & spi_bus, PinName cs);
    
    ///@brief AD5781 Destructor
    ~AD5781();

    // ///@brief AD5781 Constructor
    // ///@param[in] spi_bus - reference to SPI bus for this device
    // ///@param[in] cs - pin to be used for chip select
    // ///@param[in] interrupt - pin to be used as interrupt input, default = NC
    // ///@param[in] cnvrt - pin to be used for convert, default = NC
    // AD5781(BurstSPI & spi_bus, PinName cs);
    
    // ///@brief AD5781 Destructor
    // ~AD5781();


    void init(void);
    
    ///@brief Writes given register with data
    void write_register(AD5781RegAddress_t reg, uint32_t data);
    
    ///@brief Reads given register
    uint32_t read_register(AD5781RegAddress_t reg);
    
    

    // Custom
    void reset();
    void dac_update(uint32_t data);

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
} // namespace ad5781

#endif /* AD5781_H */