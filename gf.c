#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define GEN_POLY 0x03


unsigned char gf_log[256];
unsigned char gf_exp[256];


uint8_t gf_slow_mult (uint8_t a, uint8_t b, uint8_t poly)
{

	uint8_t out = 0;
	
	while (b) {
		if (b & 0x1) 
			out ^= a;

		a = (a << 1) ^ ((a & 0x80) ? poly : 0);

		b >>= 1;
	}

	return out;	
}


void gf_tables_init(uint8_t poly)
{
	uint8_t c;
	int i;

	c = 1;
	for (i = 0; i < 256; i++) {
		gf_log[c] = i;
		gf_exp[i] = c;
		c = gf_slow_mult(c, GEN_POLY, poly);
	}

}

uint8_t gf_fast_mult(uint8_t a, uint8_t b)
{
	int c;

	if (a == 0 || b == 0)
		return 0;

	c = gf_log[a] + gf_log[b];

	return gf_exp[c % 255];
}

