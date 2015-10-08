/*****************************************************************************/
/**
 *	@file smallprintf.c
 *
 *	@created 8. 10. 2015
 *	@author Mica (michalhrouda@gmail.com)
 *
 *	SMALLPRINTF is an lightweigth printf for the microcontrolers
 *
 *	Main feature is putchar function passed into smallprintf as its input parameter
 *	uint16_t smallprintf(void (*small_putchar)(char), char * str, ...)
 *
 *	You can use smallprintf this way:
 *	#define uartprintf(...)  smallprintf(UART_SendByte, __VA_ARGS__)
 *	#define lcdprintf(...)   smallprintf(HD44780_PutChar, __VA_ARGS__)
 *
 *	Implementation of your own putchar function is mandatory. Its format is simple:
 *	void UART_SendByte (char charater)
 *	{
 *		//Code that sends character into your peripheral
 *	}
 *
 *	Supported conversions:
 *
 *	%s - String with left and right justification
 *
 *	%x - Hexadecimal conversion, is always in uppercase and the legth of the
 *	output character is HEXA_STRING_MAX_LENGTH by default
 *
 *	%u - Unsigned number conversion, UNSIGNED_NUMBER_TYPE parameter is expected
 *	maximal number length is specified by the DECIMAL_STRING_MAX_LENGTH
 *
 *	%d - Signed number conversion, SIGNED_NUMBER_TYPE parameter is expected
 *	maximal number length is specified by the DECIMAL_STRING_MAX_LENGTH
 *
 *	%% - % character is printed
 *
 *	@licence CC
 *
 *****************************************************************************/


#include "smallprintf.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "uart.h"



#define UNSIGNED_NUMBER_TYPE 		uint16_t
#define SIGNED_NUMBER_TYPE 			int16_t
#define DECIMAL_STRING_MAX_LENGTH 	5
#define HEXA_STRING_MAX_LENGTH 		4

#define JUSTIFY_RIGHT				0
#define JUSTIFY_LEFT				1


/*****************************************************************************/
/**
 * @brief Number to decadic string procedure
 *
 * @param val - Number to be printed
 *
 *****************************************************************************/
uint16_t smallprintf_decadic(void (*small_putchar)(char), UNSIGNED_NUMBER_TYPE val, uint8_t digits)
{
	uint16_t bytes_written = 0;

	uint8_t tempchar[DECIMAL_STRING_MAX_LENGTH];
	uint8_t i;
	uint8_t non_zero_found = 0;


	for (i = 0; i < DECIMAL_STRING_MAX_LENGTH ; i++)
	{
		UNSIGNED_NUMBER_TYPE temp = val % 10;
		val = val / 10;
		tempchar[DECIMAL_STRING_MAX_LENGTH - 1 - i] = temp;
	}

	for (i = 0; i < DECIMAL_STRING_MAX_LENGTH ; i++)
	{
		if ((DECIMAL_STRING_MAX_LENGTH - digits) <= i)
		{
			non_zero_found = 1;
		}

		if (non_zero_found == 0)
		{
			if (tempchar[i] > 0)
			{
				non_zero_found = 1;
			}
			else
			{
				continue;
			}
		}
		small_putchar (tempchar[i] + '0');
		bytes_written++;
	}

	return bytes_written;
}


/*****************************************************************************/
/**
 * @brief Number to hexadecimal string procedure
 *
 * @param val - Number to be printed
 * @prama digits - Number of hexadecimal digits
 *
 *****************************************************************************/
uint16_t smallprintf_hexadecimal(void (*small_putchar)(char), UNSIGNED_NUMBER_TYPE val, uint8_t digits)
{
	uint16_t bytes_written = 0;

	uint8_t tempchar[HEXA_STRING_MAX_LENGTH];
	uint8_t temp;
	uint8_t i;

	for (i = 0; i < HEXA_STRING_MAX_LENGTH ; i++)
	{
		temp = (val & 0x000F);
		val = val >> 4;
		tempchar[HEXA_STRING_MAX_LENGTH - 1 - i] = temp;
	}

	for (i = HEXA_STRING_MAX_LENGTH - digits; i < HEXA_STRING_MAX_LENGTH ; i++)
	{
		temp = tempchar[i];
		small_putchar((temp > 9) ? temp + 'A' - 10: temp + '0');
		bytes_written++;
	}

	return bytes_written;
}

/*****************************************************************************/
/**
 * @brief Lightweight Printf implementation
 *
 * @param str - String to be formated and printed
 * @return Number of printed characters
 *
 * @todo fill handling
 *
 *****************************************************************************/
uint16_t smallprintf(void (*small_putchar)(char), char * str, ...)
{
	uint16_t bytes_written = 0;

	//uint8_t zerofill = 0;
	uint8_t digits = 0;
	uint8_t justify = JUSTIFY_RIGHT;
	uint8_t spaces_fill = 0;


	bytes_written = 0;

	va_list arg_list;

	va_start(arg_list, str);

	char * x = str;
	while (*x != 0)
	{
		if (*x == '%')
		{
			x++;

			if (*x == '-')
			{
				justify = JUSTIFY_LEFT
			}

			digits = 0;

			if ((*x >= '0') && (*x <= '9'))
			{
				digits = *x++ - '0';

				if ((*x >= '0') && (*x <= '9'))
				{
					digits = (digits * 10) + (*x++ - '0');
				}
			}

			switch (*x)
			{

				case 'x':
				{
					x++;

					if ((digits == 0) ||  (digits > HEXA_STRING_MAX_LENGTH))
					{
						digits = HEXA_STRING_MAX_LENGTH;
					}

					UNSIGNED_NUMBER_TYPE val = va_arg(arg_list, UNSIGNED_NUMBER_TYPE);
					bytes_written += smallprintf_hexadecimal(small_putchar, val, digits);

					break;
				}


				case 'd':
				{
					x++;

					SIGNED_NUMBER_TYPE val = va_arg(arg_list, SIGNED_NUMBER_TYPE);
					if (val < 0)
					{
						small_putchar('-');
						bytes_written++;
						val = -val;
					}
					bytes_written += smallprintf_decadic(small_putchar, val, digits);
					break;
				}


				case 'u':
				{
					x++;
					UNSIGNED_NUMBER_TYPE val = va_arg(arg_list, UNSIGNED_NUMBER_TYPE);
					bytes_written += smallprintf_decadic(small_putchar, val, digits);
					break;
				}


				case 's':
				{
					x++;
					char * val = va_arg(arg_list, char *);

					if (val != 0)
					{
						if (digits > 0)
						{
							char length = strlen(val);
							if (length < digits)
							{
								spaces_fill = digits - length;
							}
						}

						if (justify == JUSTIFY_LEFT)
						{
							while (spaces--) small_putchar (' ');
						}

						while (*val != 0)
						{
							small_putchar (*val++);
							bytes_written++;
						}

						if (justify == JUSTIFY_RIGHT)
						{
							while (spaces--) small_putchar (' ');
						}
					}
					break;
				}


				case '%':
				{
					x++;
					small_putchar ('%');
					bytes_written++;
					break;
				}

				default:
				{
					small_putchar(*x++);
					bytes_written++;
					break;
				}
			}

		}
		else
		{
			small_putchar(*x++);
			bytes_written++;
		}
	}

	return bytes_written;
}
