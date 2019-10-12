#include <char2hex.h>
/* Assuming a char is 8 bits i.e 1 byte.
 * This is hard coded but respects the ANSI C 
 * specification which ensures that a char is atleast
 * 1 byte in size.
 * For optimization the loop is unrolled here so it may 
 * seem big but its actually 4 lines of code.  */
char *char2hex(unsigned char c){
	static char buffer[5] = {0};
	char *buf = buffer;
	*buf++ = '\\';
	*buf++ = 'x';
	*buf = (c >> 4) & 0xf;
	*buf += (*buf < 10) ? '0' : 'A' - 10;
	buf++;
	*buf = c & 0xf; /* the least significant byte. */
	*buf += (*buf < 10) ? '0' : 'A' - 10;
	*(++buf) = '\0';
	return buffer;
}

