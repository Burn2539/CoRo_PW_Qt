#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"

/*************************************************************************
* Source:	Programming Embedded Systems in C and C++
*			By Michael Barr, p.76 to p.79
**************************************************************************/

/*
* The CRC parameters. Currently configured for CCITT.
* Simply modify these to switch to another CRC standard.
*/
#define POLYNOMIAL			0x1021
#define INTIAL_REMAINDER	0xFFFF
#define FINAL_XOR_VALUE		0x0000

/*
* The width of the CRC calculation and result.
* Modify the typedef for an 8 or 32-bit CRC standard.
*/
typedef unsigned short width;

#define WIDTH	(8 * sizeof(width))
#define TOPBIT	(1 << (WIDTH - 1))


/*************************************************************************
*  PROTOTYPES
**************************************************************************/

void crcInit(void);
width crcCompute(unsigned char * message, unsigned int nBytes);
quint32 encodeCRC(quint16 value);
bool verifyCRC(quint16 value, quint16 good_crc);
