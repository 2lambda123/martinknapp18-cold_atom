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
GPOCON = 0x05,

/// 0x07 r/o Offset register
OFFSET = 0x06,

/// 0x08 r/o Full-scale register
FULLSCALE = 0x07,

} AD7195RegSelect_t;

/// Define register sizes
#define COMM_SIZE 	    1
#define STATUS_SIZE 	1
#define MODE_SIZE 	    3
#define CONFIG_SIZE 	3
#define DATA_SIZE 	    3
#define ID_SIZE 	    1
#define GPOCON_SIZE 	1
#define OFFSET_SIZE 	3
#define FULLSCALE_SIZE  3

/// DESIGN
#define STATUS_DESIGN 	    0x00
#define MODE_DESIGN 	    0x080001
#define CONFIG_DESIGN 	    0x001008
#define DATA_DESIGN 	    0x000000
#define ID_DESIGN 	        0x00
#define GPOCON_DESIGN 	    0x00
#define OFFSET_DESIGN 	    0x000000
#define FULLSCALE_DESIGN    0x000000

#endif /* _AD7195_DESIGNVALUE_H_ */

// End of file
