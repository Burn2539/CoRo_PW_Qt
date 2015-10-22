/*************************************************************************
* Source:	Programming Embedded Systems in C and C++
*			By Michael Barr, p.76
**************************************************************************/

#include "crc.h"

/*
* An array containing the pre-computed intermediate result for each
* possible byte of input. This is used to speed up the computation.
*/
width crcTable[256];

/*************************************************************************
*
* Function:		crcInit()
*
* Description:	Initialize the CRC lookup table. This table is used
*				by crcCompute() to make CRC computation faster.
*
* Notes:		The mod-2 binary long division is implemented here.
*
* Parameters:	None.
*
* Returns:		None defined.
*
**************************************************************************/

void crcInit(void)
{
	width	remainder;
	width	dividend;
	int		bit;

	/* Perform binary long division, a bit at a time. */
	for (dividend = 0; dividend < 256; dividend++)
	{
		/* Initialize the remainder */
		remainder = dividend << (WIDTH - 8);

		/*Shift and XOR with the polynomial.*/
		for (bit = 0; bit < 8; bit++)
		{
			/* Try to divide the current data bit. */
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = remainder << 1;
			}
		}

		/* Save the result in the table. */
		crcTable[dividend] = remainder;
	}

} /* crcInit() */


/*************************************************************************
*
* Function:		crcCompute()
*
* Description:	Compute the CRC checksum of a binary message block.
*	
* Notes:		This function expects that crcInit() has been called
*				first to intialize the CRC lookup table.
*
* Parameters:	Pointer to the message.
*				Number of bytes of the message.
*
* Returns:		The CRC of the data.
*
**************************************************************************/

width crcCompute(unsigned char * message, unsigned int nBytes)
{
	unsigned int	offset;
	unsigned char	byte;
	width			remainder = INTIAL_REMAINDER;

	/* Divide the message by the polynomial, a byte at a time. */
	for (offset = 0; offset < nBytes; offset++)
	{
		byte = (remainder >> (WIDTH - 8)) ^ message[offset];
		remainder = crcTable[byte] ^ (remainder << 8);
	}

	/* The final remainder is the CRC result. */
	return (remainder ^ FINAL_XOR_VALUE);

} /* crcCompute() */


/*************************************************************************
*
* Function:		encodeCRC()
*
* Description:	Add the CRC at the end of the message.
*
* Notes:		Works with a original message of 16 bits.
*
* Parameters:	Value to encode.
*
* Returns:		32 bits message. Message (16 bits) followed by
*				the CRC (16 bits).
*
**************************************************************************/

quint32 encodeCRC(quint16 value)
{
	const int		SizeOfMessage = sizeof(value);
	unsigned char 	message[SizeOfMessage];
	width			crc;
    quint32         message_with_crc;
	int				i;

	/* Convert the uint16 into a array of unsigned chars. */
	for (i = 0; i < SizeOfMessage; i++)
	{
		message[SizeOfMessage - 1 - i] = (value >> (i * 8) & 0xFF);
	}

	/* Calculate the CRC. */
	crc = crcCompute(message, SizeOfMessage);

	/* Concatenate the original message with the CRC. */
	message_with_crc = value << 8 * SizeOfMessage | crc;

	return message_with_crc;

} /* encodeCRC() */


/*************************************************************************
*
* Function:		verifyCRC()
*
* Description:	Verify that the message is valid according to the CRC.
*
* Notes:
*
* Parameters:	Message to verify.
*				CRC of the message ot verify.
*
* Returns:		FALSE if checkSUM is bad.
*				TRUE if checkSUM is good.
*
**************************************************************************/

bool verifyCRC(quint16 value, quint16 good_crc)
{
	const int		SizeOfMessage = sizeof(value);
	unsigned char 	message[SizeOfMessage];
	width			crc;
	int				i;

	/* Convert the uint16 into a array of unsigned chars. */
	for (i = 0; i < SizeOfMessage; i++)
	{
		message[SizeOfMessage - 1 - i] = (value >> (i * 8) & 0xFF);
	}

	/* Calculate the CRC. */
	crc = crcCompute(message, SizeOfMessage);

	/* Validate the CRC received with the CRC calulated. */
	return (crc == good_crc);

} /* verifyCRC() */
