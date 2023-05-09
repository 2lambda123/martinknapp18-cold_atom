#include "AD5781.h"
#include "cycle_count_delay.h"
#include "BurstSPI.h"


namespace {
constexpr uint32_t AD5781_SPI_RATE = 10000000; // Hz. SPI_1 comes from APB2CLK = 90 MHz, prescaler 4 = 22.5 MHz, 35 MHz for write, 16 MHz for read
constexpr uint32_t AD5781_SPI_RATE_READ = 16000000; // 16 MHz for read

// uint32_t reg_data_buf[3];
// uint32_t rtn_val_buf[3];

} // namespace


namespace drivers_AD {
namespace ad5781 {



//*********************************************************************
AD5781::AD5781(SPI & spi_bus, PinName cs):
    m_spi_bus(spi_bus), m_cs(cs, 1), m_write_done(0)
{
    init();
}

//*********************************************************************
AD5781::~AD5781()
{
}


// //*********************************************************************
// AD5781::AD5781(BurstSPI & spi_bus, PinName cs):
//     m_spi_bus(spi_bus), m_cs(cs, 1), m_write_done(0)
// {
//     init();
// }

// //*********************************************************************
// AD5781::~AD5781()
// {
// }

//*********************************************************************
void AD5781::spi_write_cb(int event) {
    m_cs = 1;
    m_write_done = 1;
}

//*********************************************************************
void AD5781::write_register(AD5781RegAddress_t reg, uint32_t data)
{
    uint32_t reg_data_buf[3];
    reg_data_buf[0] = ((0xF0000 & data) >> 16);
    reg_data_buf[1] = ((0x0FF00 & data) >> 8);
    reg_data_buf[2] = ((0x000FF & data));

    m_cs = 0;
    m_spi_bus.write( AD5781Addr_SPI_Write(reg) | reg_data_buf[0] );
    m_spi_bus.write(reg_data_buf[1]);
    m_spi_bus.write(reg_data_buf[2]);
    m_cs = 1;

}

// *********************************************************************    
uint32_t AD5781::read_register(AD5781RegAddress_t reg)
{
    uint32_t rtn_val = 0;

    m_cs = 0;
    m_spi_bus.write(AD5781Addr_SPI_Read(reg));
    m_spi_bus.write(0x00);
    m_spi_bus.write(0x00);
    m_cs = 1;

    // cycle_delay_ms(1);

    m_cs = 0;
    rtn_val |= (m_spi_bus.write(AD5781Addr_SPI_Write(NOP)) << 16);
    rtn_val |= (m_spi_bus.write(0xFF) << 8);
    rtn_val |= m_spi_bus.write(0xFF);
    m_cs = 1;

    return rtn_val;
}

//*********************************************************************
void AD5781::dac_update(uint32_t data)
{
    // write to the DAC register
    write_register(DACreg, data);

    return;
}


//*********************************************************************
void AD5781::reset(void)
{
    m_cs = 0;
    m_spi_bus.write(AD5781Addr_SPI_Write(SOFTWARE));
    m_spi_bus.write(0x00);
    m_spi_bus.write(0x04);
    m_cs = 1;
}


//*********************************************************************
void AD5781::init(void)
{
    m_spi_bus.format(8,2);
    m_spi_bus.frequency(AD5781_SPI_RATE);

    cycle_delay_ms(1);
    reset();
    cycle_delay_ms(1); // need to wait minimum of 500us before addressing serial interface

    // Configure the CONTROL register
    read_register(CONTROL);
    cycle_delay_us(10);
    write_register(CONTROL, CONTROL_DESIGN_SDO);
    cycle_delay_us(10);
    read_register(CONTROL);
    cycle_delay_us(10);

}

} // namespace drivers
} // namespace ad5781
