#include "AD7195.h"
#include "cycle_count_delay.h"
#include "BurstSPI.h"


namespace {
constexpr uint32_t AD7195_SPI_RATE = 5000000; // Hz. SPI_1 comes from APB2CLK = 90 MHz, prescaler 4 = 22.5 MHz
uint32_t reg_data_buf[3];
// uint32_t rtn_val_buf[3];

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
// AD7195::AD7195(BurstSPI & spi_bus, PinName cs):
//     m_spi_bus(spi_bus), m_cs(cs, 1), m_write_done(0)
// {
//     init();
// }

// //*********************************************************************
// AD7195::~AD7195()
// {
// }

//*********************************************************************
void AD7195::spi_write_cb(int event) {
  m_cs = 1;
  m_write_done = 1;
}

//*********************************************************************
void AD7195::write_register(AD7195RegSelect_t reg, uint8_t size, uint32_t data)
{
    uint8_t i = 0;
    // uint32_t reg_data_buf[size];

    if (size == 3){
        reg_data_buf[0] = ((0xFF0000 & data) >> 16);
        reg_data_buf[1] = ((0x00FF00 & data) >> 8);
        reg_data_buf[2] = (0x0000FF & data);
    }
    else{
        reg_data_buf[0] = (0xFF & data);
    }
    
    m_cs = 0;
    m_spi_bus.write(AD7195Addr_SPI_Write(reg));
    while(size--){
        m_spi_bus.write(reg_data_buf[i]);
        i++;
    }

    m_cs = 1;
}

//*********************************************************************    
uint32_t AD7195::read_register(AD7195RegSelect_t reg, uint8_t size)
{
    // uint8_t i = 0;
    uint32_t rtn_val = 0;
    // uint16_t rtn_val_buf = {0,0,0};
    // m_cs = 0;
    // m_spi_bus.write(AD7195Addr_SPI_Read(reg));
    // while(size--){
    //     m_spi_bus.write(0xFF);
    //     i++;
    // }

    m_cs = 0;
    m_spi_bus.write(AD7195Addr_SPI_Read(reg));
    rtn_val |= (m_spi_bus.write(0xFF) << 16);
    rtn_val |= (m_spi_bus.write(0xFF) << 8);
    rtn_val |= m_spi_bus.write(0xFF);
    m_cs = 1;

    if (size == 1){
        rtn_val = (rtn_val >> 16);
    }

    return rtn_val;
}

//*********************************************************************
void AD7195::max_speed_adc_read(uint32_t* values, size_t num_samples)
{
    // __disable_irq();  // disable all interrupts

    m_cs = 0;
    for(size_t i = 0; i < num_samples; i++){
        // while (PE_6 != 1);
        // values[i] = read_register(DATA, DATA_SIZE);
        // cycle_delay_us(100);

        uint32_t rtn_val = 0;
        // while (PE_6 != 1);
        while (PE_6 == 0);
        m_spi_bus.write(AD7195Addr_SPI_Read(DATA));
        rtn_val |= (m_spi_bus.write(0xFF) << 16);
        rtn_val |= (m_spi_bus.write(0xFF) << 8);
        rtn_val |= m_spi_bus.write(0xFF);

        values[i] = rtn_val;


    }
    m_cs = 1;

    // __enable_irq();  // disable all interrupts
}

//*********************************************************************
uint32_t AD7195::read_dev_id()
{
    uint32_t rtn_val = 0;
    rtn_val = read_register(ID, ID_SIZE);
    // m_cs = 0;
    // m_spi_bus.write(AD7195Addr_SPI_Read(ID));
    // rtn_val = m_spi_bus.write(0xFF);
    // m_cs = 1;
    return rtn_val;

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
    m_spi_bus.format(8,3);
    m_spi_bus.frequency(AD7195_SPI_RATE);

    cycle_delay_ms(1);
    reset();
    cycle_delay_ms(1); // need to wait minimum of 500us before addressing serial interface

    // Configure Registers
    write_register(MODE, MODE_SIZE, MODE_DESIGN);
    write_register(CONFIG, CONFIG_SIZE, CONFIG_DESIGN);
    write_register(GPOCON, GPOCON_SIZE, GPOCON_DESIGN);


}

} // namespace drivers
} // namespace ad7195
