/**********************************************************************
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/

#include "max11300.h"
#include "cycle_count_delay.h"
#include "BurstSPI.h"
#include <cstdio>


namespace {
constexpr uint32_t MAX11300_SPI_RATE = 18000000; // Hz. SPI_1 comes from APB2CLK = 90 MHz, prescaler 4 = 22.5 MHz

// uint16_t reg_data_buf[3];

// The following buffer will be precomputed and saved for ramps to properly ramp
// fast enough. Doing the computation on the fly takes too much time.
// all multiplied by 2 becuase for each ramp element i save two bits of information
// [port_number, data_0, port_number, data_1, ...]
// Current usage in the cold_atom experiment:
// 2 * (2 * 10 + X * X) = x2 bits of data * (no. of things simultaneously ramped * no. of steps)
// the plus X * X would represent another ramp
// this needs to be changed manually becuase it is not computed on the fly
constexpr uint16_t RAMP_BUFFER_SIZE = 2 * ( (2 * 10) );
uint16_t ramp_buffer[RAMP_BUFFER_SIZE];
uint16_t optimise_ramp_buffer[RAMP_BUFFER_SIZE];
// uint16_t ramp_offset_array[10];

DigitalIn  ADC_INT(PE_11, PullUp);

} // namespace


namespace drivers {
namespace max11300 {


static const uint16_t port_config_design_vals[20] = {
    port_cfg_00_DESIGNVALUE,
    port_cfg_01_DESIGNVALUE,
    port_cfg_02_DESIGNVALUE,
    port_cfg_03_DESIGNVALUE,
    port_cfg_04_DESIGNVALUE,
    port_cfg_05_DESIGNVALUE,
    port_cfg_06_DESIGNVALUE,
    port_cfg_07_DESIGNVALUE,
    port_cfg_08_DESIGNVALUE,
    port_cfg_09_DESIGNVALUE,
    port_cfg_10_DESIGNVALUE,
    port_cfg_11_DESIGNVALUE,
    port_cfg_12_DESIGNVALUE,
    port_cfg_13_DESIGNVALUE,
    port_cfg_14_DESIGNVALUE,
    port_cfg_15_DESIGNVALUE,
    port_cfg_16_DESIGNVALUE,
    port_cfg_17_DESIGNVALUE,
    port_cfg_18_DESIGNVALUE,
    port_cfg_19_DESIGNVALUE};

// //*********************************************************************
// MAX11300::MAX11300(SPI & spi_bus, PinName cs, PinName interrupt, PinName cnvt):
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
MAX11300::MAX11300(BurstSPI & spi_bus, PinName cs, PinName interrupt, PinName cnvt):
    m_spi_bus(spi_bus), m_cs(cs, 1), m_int(interrupt), m_cnvt(cnvt, 1),
    m_write_done(0)
{
    init();
}

//*********************************************************************
MAX11300::~MAX11300()
{
}

//*********************************************************************
void MAX11300::spi_write_cb(int event) {
  m_cs = 1;
  m_write_done = 1;
}

//*********************************************************************
void MAX11300::write_register(MAX11300RegAddress_t reg, uint16_t data)
{
    // m_cs = 0;
    // m_spi_bus.write(MAX11300Addr_SPI_Write(reg));
    // m_spi_bus.write(((0xFF00 & data) >> 8));
    // m_spi_bus.write((0x00FF & data));
    // m_cs = 1;
    // wait_us(80);

    m_cs = 0;
    m_spi_bus.fastWrite_three_byte((MAX11300Addr_SPI_Write(reg)<<16) | (data));
    m_cs = 1;
    m_spi_bus.clearRX();
    wait_us(80);
}

//*********************************************************************    
uint16_t MAX11300::read_register(MAX11300RegAddress_t reg)
{
    uint16_t rtn_val = 0;
    
    // m_cs = 0;
    // m_spi_bus.write(MAX11300Addr_SPI_Read(reg));
    // rtn_val |= (m_spi_bus.write(0xFF) << 8);
    // rtn_val |= m_spi_bus.write(0xFF);
    // m_cs = 1;

    __disable_irq();  // disable all interrupts
    m_spi_bus.clearRX();
    m_cs = 0;
    rtn_val = m_spi_bus.fastRead(MAX11300Addr_SPI_Read(reg));
    m_cs = 1;
    __enable_irq();  // disable all interrupts
    // m_spi_bus.clearRX();
    
    return rtn_val;
}

//*********************************************************************    
void MAX11300::block_write(MAX11300RegAddress_t reg, uint16_t * data, uint8_t num_reg)
{
    for(uint8_t idx = 0; idx < num_reg; idx++)
    {
        write_register(static_cast<MAX11300RegAddress_t>(reg + idx), data[idx]);
    }
}

//*********************************************************************        
void MAX11300::block_read(MAX11300RegAddress_t reg, uint16_t * data, uint8_t num_reg)
{
    for(uint8_t idx = 0; idx < num_reg; idx++)
    {
        data[idx] = read_register(static_cast<MAX11300RegAddress_t>(reg + idx));
    }
}

//*********************************************************************    
MAX11300::CmdResult MAX11300::gpio_write(MAX11300_Ports port, uint8_t state)
{
    MAX11300::CmdResult result = MAX11300::OpFailure;
    uint16_t temp;
    uint16_t port_mask;
    
    if(((port_config_design_vals[port] & 0xF000) >> 12) == MAX11300::MODE_3)
    {
        if(port < MAX11300::PORT16)
        {
            port_mask = (1 << port);
            temp = read_register(gpo_data_15_to_0);
            if(state & 0x01)
            {
                temp |= port_mask;
            }
            else
            {
                temp &= ~port_mask;
            }
            write_register(gpo_data_15_to_0, temp);
        }
        else
        {
            port_mask = (1 << (port - MAX11300::PORT16));
            temp = read_register(gpo_data_19_to_16);
            if(state & 0x01)
            {
                temp |= port_mask;
            }
            else
            {
                temp &= ~port_mask;
            }
            write_register(gpo_data_19_to_16, temp);
        }
        
        result = MAX11300::Success;
    }
    
    return result;
}

//*********************************************************************
MAX11300::CmdResult MAX11300::gpio_read(MAX11300_Ports port, uint8_t & state)
{
    MAX11300::CmdResult result = MAX11300::OpFailure;
    
    if(((port_config_design_vals[port] & 0xF000) >> 12) == MAX11300::MODE_1)
    {
        if(port < MAX11300::PORT16)
        {
            state = (read_register(gpi_data_15_to_0) >> port);
        }
        else
        {
            state = (read_register(gpi_data_19_to_16) >> (port - MAX11300::PORT16));
        }
        
        result = MAX11300::Success;
    }
    
    return result;
}

//*********************************************************************
MAX11300::CmdResult MAX11300::single_ended_adc_read(MAX11300_Ports port, uint16_t & data)
{
    MAX11300::CmdResult result = MAX11300::OpFailure;
    
    if(((port_config_design_vals[port] & 0xF000) >> 12) == MAX11300::MODE_7)
    {
        uint8_t num_samples = ((port_config_design_vals[port] & port_cfg_00_funcprm_nsamples) >> 5);
        num_samples = (1 << num_samples);
        
        while(num_samples--)
        {
            m_cnvt = 0;
            wait_us(1);
            m_cnvt = 1;
            wait_us(100);
        }
        data = read_register(static_cast<MAX11300RegAddress_t>(adc_data_port_00 + port));
        
        result = MAX11300::Success;
    }
    
    return result;
}

//*********************************************************************
MAX11300::CmdResult MAX11300::single_ended_dac_write(MAX11300_Ports port, uint16_t data)
{
    MAX11300::CmdResult result = MAX11300::OpFailure;
    
    if(((port_config_design_vals[port] & 0xF000) >> 12) == MAX11300::MODE_5)
    {
        write_register(static_cast<MAX11300RegAddress_t>(dac_data_port_00 + port) , data);
        result = MAX11300::Success;
    }
    
    return result;
}

//*********************************************************************
void MAX11300::prepare_ramps(RampAction *ramp_action, Ramp *ramps)
{
    // check if the ramp already configured
    if (!ramp_action->configured){
        ramp_action->ramp_id = &ramp_buffer[ramp_offset];
    }

    for (uint16_t i=0; i < ramp_action->num_steps; i++){
        // printf("%i\n\r", i);
        for (uint16_t j=0; j < ramp_action->num_ramps; j++){
            Ramp ramp = ramps[j];

            // Calculate step size (converted to int which truncates any decimal places)
            float step_size = (ramp.end_dac - ramp.start_dac) / static_cast<float>(ramp_action->num_steps-1);
            uint16_t dac_value = round(ramp.start_dac + ((i) * step_size));

            // add the data, and the port number [P#_1, data_1, P#_2, data_2]
            ramp_action->ramp_id[ 2 * (i * ramp_action->num_ramps + j) ] = ramp.port;
            ramp_action->ramp_id[ 2 * (i * ramp_action->num_ramps + j) + 1 ] = dac_value;
            // printf("%i - %i - %.2f \n\r", ramp.port, dac_value, step_size);
        }
    // printf("\n\r");
    }

    uint32_t written_data_size = 2 * (ramp_action->num_ramps * ramp_action->num_steps);
    // once ramp data is written to ramp_buffer, we increment ramp_offset so as not overwrite when we prepare another ramp
    if (!ramp_action->configured) {
        ramp_offset += written_data_size;
        ramp_action->configured = 1;
    }

}


//*********************************************************************
void MAX11300::prepare_optimise_ramps(RampAction *ramp_action, Ramp *ramps)
{
    ramp_action->ramp_id = &optimise_ramp_buffer[0];
    for (uint16_t i=0; i < ramp_action->num_steps; i++){
        // printf("%i\n\r", i);
        for (uint16_t j=0; j < ramp_action->num_ramps; j++){
            Ramp ramp = ramps[j];

            // Calculate step size (converted to int which truncates any decimal places)
            float step_size = (ramp.end_dac - ramp.start_dac) / static_cast<float>(ramp_action->num_steps-1);
            uint16_t dac_value = round(ramp.start_dac + ((i) * step_size));

            // add the data, and the port number [P#_1, data_1, P#_2, data_2]
            ramp_action->ramp_id[ 2 * (i * ramp_action->num_ramps + j) ] = ramp.port;
            ramp_action->ramp_id[ 2 * (i * ramp_action->num_ramps + j) + 1 ] = dac_value;
            // printf("%i - %i - %.2f \n\r", ramp.port, dac_value, step_size);
        }
    // printf("\n\r");
    }
}


//*********************************************************************
void MAX11300::run_ramps(RampAction *ramp_action)
{
    uint16_t *ramp_buffer = ramp_action->ramp_id;
    // printf("%i\n\r", *ramp_action->ramp_id);

    // loop over every element of combined ramps
    for (uint16_t i=0; i < ramp_action->num_steps; i++){
        // loop over the number of ramps
        for (uint16_t j=0; j < ramp_action->num_ramps; j++){
            // printf("%i\n\r", i);
            MAX11300_Ports port = (MAX11300_Ports)ramp_buffer[ (2 * (i * ramp_action->num_ramps + j)) ];
            uint16_t data = ramp_buffer[ (2 * (i * ramp_action->num_ramps + j) + 1) ];
            single_ended_dac_write(port, data);
            cycle_delay_us(ramp_action->step_time_us);

            // printf("%i - %i - %i \n\r", i, port, data);
            // printf("\n\r");
            // cycle_delay_ms(250);
        }
    }

}

//*********************************************************************
void MAX11300::max_speed_adc_read(MAX11300_Ports port, uint16_t* values, size_t num_samples)
{
    // __disable_irq();  // disable all interrupts
    for(size_t i = 0; i < num_samples; i++){

        // while (ADC_INT != 1);
        values[i] = read_register(static_cast<MAX11300RegAddress_t>(adc_data_port_00 + port));

    }
    // __enable_irq();  // disable all interrupts
}

//*********************************************************************
void MAX11300::read_dev_id()
{
    uint16_t address = 0;
    address = read_register(static_cast<MAX11300RegAddress_t>(0x00));
    // printf("%u\n\r", address);
}

//*********************************************************************
void MAX11300::init(void)
{
    m_spi_bus.format(8,0);
    m_spi_bus.frequency(MAX11300_SPI_RATE);

    wait_us(100000);
    write_register(device_control, 0x8000);
    wait_us(100000);

    //see datasheet 19-7318; Rev 3; 4/16; page 49
    //https://datasheets.maximintegrated.com/en/ds/MAX11300.pdf
    //for description of configuration process
    
    uint8_t idx;
    uint8_t port_mode;
    uint16_t mode_bit_mask = 0;
    
    //figure out port modes used
    for(idx = 0; idx < 20; idx++)
    {
        port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12);
        if(port_mode > 0)
        {
            mode_bit_mask |= (1 << port_mode);
        }   
    }
    
    //STEP 1: configure BRST, THSHDN, ADCCONV
    uint16_t device_control_local = (device_control_DESIGNVALUE & (device_control_BRST | device_control_THSHDN | device_control_ADCCONV));
    write_register(device_control, device_control_local);
    
    //STEP 2: If any port is configured for modes 1,3,4,5,6, or 10
    if(mode_bit_mask & MODE_BITMASK_PROCESS_1)
    {
        config_process_1(device_control_local);
    }
    
    //STEP 3: If any port is configured for modes 7,8, or 9
    if(mode_bit_mask & MODE_BITMASK_PROCESS_2)
    {
        config_process_2(device_control_local);
    }
    
    //STEP 4: If any port is configured for modes 2,11, or 12
    if(mode_bit_mask & MODE_BITMASK_PROCESS_3)
    {
        config_process_3();
    }
    
    //STEP 5: Are Temperature sensors used?
    if(device_control_DESIGNVALUE & (device_control_TMPCTLEXT1 | device_control_TMPCTLEXT0 | device_control_TMPCTLINT))
    {
        device_control_local |= (device_control_DESIGNVALUE & (device_control_TMPPER | device_control_RS_CANCEL));
        write_register(device_control, device_control_local);
        
        uint16_t temp_thresholds [6] = {
            tmp_mon_int_hi_thresh_DESIGNVALUE, 
            tmp_mon_int_lo_thresh_DESIGNVALUE,
            tmp_mon_ext1_hi_thresh_DESIGNVALUE,
            tmp_mon_ext1_lo_thresh_DESIGNVALUE,
            tmp_mon_ext2_hi_thresh_DESIGNVALUE,
            tmp_mon_ext2_lo_thresh_DESIGNVALUE};
        block_write(tmp_mon_int_hi_thresh, temp_thresholds, 6);
        
        device_control_local |= (device_control_DESIGNVALUE & (device_control_TMPCTLEXT1 | device_control_TMPCTLEXT0 | device_control_TMPCTLINT));
        write_register(device_control, device_control_local);
    } 
    
    //STEP 6: Configure interrupt masks
    write_register(interrupt_mask, interrupt_mask_DESIGNVALUE);

    //STEP 7: Read DEV ID
    read_dev_id();
}

//*********************************************************************
void MAX11300::config_process_1(uint16_t & device_control_local)
{
    uint8_t idx;
    uint16_t port_mode;
    uint16_t dac_data_array[20] = {
        dac_data_port_00_DESIGNVALUE,
        dac_data_port_01_DESIGNVALUE,
        dac_data_port_02_DESIGNVALUE,
        dac_data_port_03_DESIGNVALUE,
        dac_data_port_04_DESIGNVALUE,
        dac_data_port_05_DESIGNVALUE,
        dac_data_port_06_DESIGNVALUE,
        dac_data_port_07_DESIGNVALUE,
        dac_data_port_08_DESIGNVALUE,
        dac_data_port_09_DESIGNVALUE,
        dac_data_port_10_DESIGNVALUE,
        dac_data_port_11_DESIGNVALUE,
        dac_data_port_12_DESIGNVALUE,
        dac_data_port_13_DESIGNVALUE,
        dac_data_port_14_DESIGNVALUE,
        dac_data_port_15_DESIGNVALUE,
        dac_data_port_16_DESIGNVALUE,
        dac_data_port_17_DESIGNVALUE,
        dac_data_port_18_DESIGNVALUE,
        dac_data_port_19_DESIGNVALUE};
    
    device_control_local |= (device_control_DESIGNVALUE & (device_control_DACREF | device_control_DACCTL));
    write_register(device_control, device_control_local);
    
    wait_us(200);
    
    //Is DACCTL = 2 or 3
    if(((device_control_DESIGNVALUE & device_control_DACCTL) == 2) || ((device_control_DESIGNVALUE & device_control_DACCTL) == 3))
    {
        //yes
        write_register(dac_preset_data_1, dac_preset_data_1_DESIGNVALUE);
        write_register(dac_preset_data_2, dac_preset_data_2_DESIGNVALUE);
    }
    else
    {
        //no
        for(idx = 0; idx < 20; idx++)
        {
            port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12);
            if((port_mode == MAX11300::MODE_1) || (port_mode == MAX11300::MODE_3) || 
               (port_mode == MAX11300::MODE_4) || (port_mode == MAX11300::MODE_5) || 
               (port_mode == MAX11300::MODE_6) || (port_mode == MAX11300::MODE_10))
            {
                write_register(static_cast<MAX11300RegAddress_t>(dac_data_port_00 + idx), dac_data_array[idx]);
            }
        }
    }
    
    //Config FUNCID[i], FUNCPRM[i] for ports in mode 1
    uint8_t num_ports_mode_1 = 0;
    for(idx = 0; idx < 20; idx++)
    {
        port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12);
        if(port_mode == MAX11300::MODE_1)
        {
            write_register(static_cast<MAX11300RegAddress_t>(port_cfg_00 + idx), port_config_design_vals[idx]);
            num_ports_mode_1++;
        }
    }
    
    wait_us(200 * num_ports_mode_1);
    
    //Config GPODAT[i] for ports in mode 3
    write_register(gpo_data_15_to_0, gpo_data_15_to_0_DESIGNVALUE);
    write_register(gpo_data_19_to_16, gpo_data_19_to_16_DESIGNVALUE);
    
    //Config FUNCID[i], FUNCPRM[i] for ports in mode 3, 4, 5, 6, or 10
    for(idx = 0; idx < 20; idx++)
    {
        port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12);
        if((port_mode == MAX11300::MODE_3) | (port_mode == MAX11300::MODE_4) | 
           (port_mode == MAX11300::MODE_5) | (port_mode == MAX11300::MODE_6) | 
           (port_mode == MAX11300::MODE_10))
        {
            write_register(static_cast<MAX11300RegAddress_t>(port_cfg_00 + idx), port_config_design_vals[idx]);
            wait_us(1000);
        }
    }
    
    //Config GPIMD[i] for ports in mode 1
    write_register(gpi_irqmode_7_to_0, gpi_irqmode_7_to_0_DESIGNVALUE);
    write_register(gpi_irqmode_15_to_8, gpi_irqmode_15_to_8_DESIGNVALUE);
    write_register(gpi_irqmode_19_to_16, gpi_irqmode_19_to_16_DESIGNVALUE);   
}

//*********************************************************************
void MAX11300::config_process_2(uint16_t & device_control_local)
{
    uint8_t idx;
    uint16_t port_mode;
    
    for(idx = 0; idx < 20; idx++)
    {
        port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12); 
        if(port_mode == MAX11300::MODE_9)
        {
            write_register(static_cast<MAX11300RegAddress_t>(port_cfg_00 + idx), port_config_design_vals[idx]);
            wait_us(100);
        }
    }
    
    for(idx = 0; idx < 20; idx++)
    {
        // uint8_t port = port_cfg_00 + idx;
        port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12); 
        if((port_mode == MAX11300::MODE_7) || (port_mode == MAX11300::MODE_8))
        {
            write_register(static_cast<MAX11300RegAddress_t>(port_cfg_00 + idx), port_config_design_vals[idx]);
            // printf("port %u\n\r", port);
            wait_us(100);
        }
    }
    
    device_control_local |= (device_control_DESIGNVALUE & device_control_ADCCTL);
    write_register(device_control, device_control_local);
}

//*********************************************************************
void MAX11300::config_process_3(void)
{
    uint8_t idx;
    uint16_t port_mode;
    
    for(idx = 0; idx < 20; idx++)
    {
        port_mode = ((port_config_design_vals[idx] & 0xf000) >> 12); 
        if((port_mode == MAX11300::MODE_2) || (port_mode == MAX11300::MODE_11) || (port_mode == MAX11300::MODE_12))
        {
            write_register(static_cast<MAX11300RegAddress_t>(port_cfg_00 + idx), port_config_design_vals[idx]);
        }
    }
}

} // namespace drivers
} // namespace max11300
