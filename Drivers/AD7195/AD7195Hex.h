#ifndef _AD7195_DESIGNVALUE_H_
#define _AD7195_DESIGNVALUE_H_

/// SPI first byte when writing AD7195 (7-bit address in bits 0x7E; LSB=0 for write)
#define AD7195Addr_SPI_Write(RegAddr) ( (RegAddr << 3)     )

/// SPI first byte when reading AD7195 (7-bit address in bits 0x7E; LSB=1 for read)
#define AD7195Addr_SPI_Read(RegAddr)  ( (RegAddr << 3) | 0x40 )

/// AD7195 Register Selection
typedef enum AD7195RegSelectEnum {

/// 0x00 r/o Communications register during a write operation
COMM = 0x00,

/// 0x01 r/o Status register during a read operation
STATUS = 0x00,

/// 0x02 r/o Mode register
MODE = 0x01,

/// 0x03 r/o Configuration register
CONFIG = 0x02,

/// 0x04 r/o Data register/data register plus status information
DATA = 0x03,

/// 0x05 r/o ID register
ID = 0x04,

/// 0x06 r/o GPOCON register
GPCON = 0x05,

/// 0x07 r/o Offset register
OFFSET = 0x06,

/// 0x08 r/o Full-scale register
FULLSCALE = 0x07,

} AD7195RegSelect_t;

/// Define register sizes
#define COMM 	0xf000
#define STATUS 	0x0c00
#define MODE 	0x0300
#define CONFIG 	0x00c0
#define DATA 	0x0030
#define ID 	    0x000c
#define GPCON 	0x0003
#define OFFSET 	0x0003
#define FULLSCALE 0x0003

#endif /* _AD7195_DESIGNVALUE_H_ */

// End of file
