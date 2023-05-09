#ifndef _AD5781_DESIGNVALUE_H_
#define _AD5781_DESIGNVALUE_H_

/// SPI first byte when writing AD5781 (MSB=0 for write)
#define AD5781Addr_SPI_Write(RegAddr) ( (RegAddr << 4) )

/// SPI first byte when reading AD5781 (MSB=1 for read)
#define AD5781Addr_SPI_Read(RegAddr)  ( (RegAddr << 4) | 0x80 )

/// AD5781 Register Selection
typedef enum AD5781RegAddressEnum {

/// 0x00 r/o No Operation register during a write operation
NOP = 0x00,

/// 0x01 r/o DAC register during a read operation, needs to be called this becuase of conflict with something else called DAC
DACreg = 0x01,

/// 0x02 r/o Control register
CONTROL = 0x02,

/// 0x03 r/o Clearcode register
CLEARCODE = 0x03,

/// 0x04 r Software register
SOFTWARE = 0x04,

} AD5781RegAddress_t;


/// Define register sizes
#define REGISTER_SIZE 	    3

/// DESIGN
#define CONTROL_DESIGN 	    0x000000
#define CONTROL_DESIGN_SDO 	0x000022



#endif /* _AD5781_DESIGNVALUE_H_ */

// End of file
