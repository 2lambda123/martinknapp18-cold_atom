#include "AD7195.h"
#include "cycle_count_delay.h"
#include "BurstSPI.h"


namespace {
constexpr uint32_t AD7195_SPI_RATE = 10000000; // Hz. SPI_1 comes from APB2CLK = 90 MHz, prescaler 4 = 22.5 MHz


} // namespace


namespace drivers_AD {
namespace ad7195 {



//*********************************************************************
AD7195::AD7195(SPI & spi_bus, PinName cs):
    m_spi_bus(spi_bus), m_cs(cs, 1), m_write_done(0)
{
    init();
}

//*********************************************************************
AD7195::~AD7195()
{
}

// //*********************************************************************
// MAX11300::MAX11300(BurstSPI & spi_bus, PinName cs, PinName interrupt, PinName cnvt):
//     m_spi_bus(spi_bus), m_cs(cs, 1), m_int(interrupt), m_cnvt(cnvt, 1),
//     m_write_done(0)
// {
//     init();
// }

// //*********************************************************************
// MAX11300::~MAX11300()
// {
// }

//*********************************************************************
void AD7195::spi_write_cb(int event) {
  m_cs = 1;
  m_write_done = 1;
}

//*********************************************************************
void AD7195::write_register(uint16_t reg, uint16_t data)
{
    m_cs = 0;
    m_spi_bus.write(reg);
    m_spi_bus.write(((0xFF00 & data) >> 8));
    m_spi_bus.write((0x00FF & data));
    m_cs = 1;

    // m_cs = 0;
    // m_spi_bus.fastWrite_three_byte((reg<<16) | (data));
    // m_cs = 1;
    // m_spi_bus.clearRX();
    // wait_us(80);
}

//*********************************************************************    
uint16_t AD7195::read_register(uint16_t reg)
{
    uint16_t rtn_val = 0;
    
    m_cs = 0;
    m_spi_bus.write(reg);
    rtn_val |= (m_spi_bus.write(0xFF) << 8);
    rtn_val |= m_spi_bus.write(0xFF);
    m_cs = 1;

    // m_spi_bus.clearRX();
    // m_cs = 0;
    // rtn_val = m_spi_bus.fastRead(MAX11300Addr_SPI_Read(reg));
    // m_cs = 1;
    // // m_spi_bus.clearRX();
    
    return rtn_val;
}


//*********************************************************************
void AD7195::read_dev_id()
{
    // uint16_t address = 0;
    // address = read_register(0x60);
    // printf("%u\n\r", address);

    m_cs = 0;
    m_spi_bus.write(0x60);
    m_spi_bus.write(0xFF);
    m_cs = 1;
}


void AD7195::reset(void)
{
    uint8_t registerWord[7];
    
    registerWord[0] = 0xFF;
    registerWord[1] = 0xFF;
    registerWord[2] = 0xFF;
    registerWord[3] = 0xFF;
    registerWord[4] = 0xFF;
    m_cs = 0;
    m_spi_bus.write(registerWord[0]);
    m_spi_bus.write(registerWord[1]);
    m_spi_bus.write(registerWord[2]);
    m_spi_bus.write(registerWord[3]);
    m_spi_bus.write(registerWord[4]);
    m_cs = 1;

}


//*********************************************************************
void AD7195::init(void)
{
    m_spi_bus.format(8,2);
    m_spi_bus.frequency(AD7195_SPI_RATE);

    cycle_delay_us(1);
    // reset(); 
}

} // namespace drivers
} // namespace ad7195
