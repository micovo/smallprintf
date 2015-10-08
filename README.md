# Smallprintf
Smallprintf is a lightweigth ANSI C implementation of the classic GCC printf function optimalized for the microcontrolers

Main feature is putchar function passed into smallprintf as its input parameter
<pre>uint16_t smallprintf(void (*small_putchar)(char), char * str, ...)</pre>

You can use smallprintf this way:<br>
<pre>&#35;define uartprintf(...)  smallprintf(UART_SendByte, __VA_ARGS__)
&#35;define lcdprintf(...)   smallprintf(HD44780_PutChar, __VA_ARGS__)
</pre>

Smallprintf putcha format example:
<pre>
void UART_SendByte (char charater)
{
 //Code that sends character into your peripheral
}
</pre>
### Supported conversions:
%s - Text string, left and right justification supported

%x - Hexadecimal conversion, is always in uppercase and the legth of the output character is HEXA_STRING_MAX_LENGTH by default
 
%u - Unsigned number conversion, UNSIGNED_NUMBER_TYPE parameter is expected maximal number length is specified by the DECIMAL_STRING_MAX_LENGTH

%d - Signed number conversion, SIGNED_NUMBER_TYPE parameter is expected maximal number length is specified by the DECIMAL_STRING_MAX_LENGTH
 
%% - % character is printed
