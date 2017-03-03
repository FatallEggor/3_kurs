#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "functions.h"
#include "gf.h"

#define SHOWMODE	1

#if SHOWMODE == 1
	#define GENERATOR(file, c)	fscanf(file, "%2x", &c)
#else
	#define GENERATOR(file, c)	(c = fgetc(file))== EOF
#endif

#if SHOWMODE == 1
	#define SHOWPRINT(text, m)\
	do {\
		printf (#text "\n"); \
		int j;\
		for(j = 0; j < 16; j++)\
			printf ("%2X  ", m[j]);\
		printf("\n\n");\
	} while (0)
#endif
	
#ifndef SHOWPRINT
	#define SHOWPRINT
#endif



FILE *in = NULL, *out = NULL;

uint8_t bl_gen_en (uint8_t *bl, FILE *in)
{
	uint8_t *blt;
	int i, c, r = 0;

/*	if (!in){
		in = fopen(name, "r");

		if (!in){
		
			printf ("can't open en-/decrypting file \"%s\"\n", name);
			return 1;
		}
	}*/

	blt = bl;
	i = 16;

	while (i > 0){

		if (GENERATOR(in, c)){
			c =(r)? 0x00 : 0x01;// to fill bits by 0
			r = 1;//this means that we reach the EOF
		}

		*blt = c;
		blt++;
		i--;
	}

	return r;
}

uint8_t bl_gen_de(uint8_t *bl, char *name)
{
	uint8_t *blt;
	int i, c;

	if (!in){
		in = fopen(name, "r");

		if (!in){
		
			printf ("can't open decrypting file \"%s\"\n", name);
			return 1;
		}
	}

	blt = bl;
	i = 16;

	while (i > 0){

		if (GENERATOR(in, c))
			return 1;
		*blt = c;
		blt++;
		i--;
	}

	return 0;
}

uint8_t sync_gen (uint8_t *sync, char *name)
{
	int i, c;
	uint8_t *tmp;
	FILE *insync;

	insync = fopen(name, "r");
	
	if (!insync){
		
		printf ("can't open key-file \"%s\"\n", name);
		return 1;
	}

	tmp = sync;
	i = 0;

	while (i < 32){

		fscanf(insync, "%2x", &c);
		*tmp = c;
		tmp++;
		i++;
	}

	return 0;
}

uint8_t keys_gen (uint8_t keys[10][16], char *name)
{
	uint8_t C[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	FILE *inkey;
	int i, c, j;
 
	i = 0;

	inkey = fopen(name, "r"); 

	if (!inkey){
		
		printf ("can't open key-file \"%s\"\n", name);
		return 1;
	}


	while (i < 16){

		fscanf(inkey, "%2x", &c);
		keys[0][i] = c;
		i++;
	}

	i = 0;

	while (i < 16){

		fscanf(inkey, "%2x", &c);
		keys[1][i] = c;
		i++;
	}

	for (i = 0; i < 4; i++){

		memcpy(keys[2*i + 2], keys[2*i], 16);
		memcpy(keys[2*i + 3], keys[2*i + 1], 16);

		for (j = 0; j < 8; j++){
			Cfunc(8 * i + j + 1, C);
			Ffunc(C, i, keys);
			}
	}

	return 0;
}


int simple_mode_en(uint8_t keys[10][16], char *name)
{
	int lb = 0, j, c;//for last block
	uint8_t bl[16];
	FILE *out, *in;

	out = fopen("encrypted", "w");
	in = fopen(name, "r");

		if (!in){
		
			printf ("can't open en-/decrypting file \"%s\"\n", name);
			return 1;
		}

	while (!lb){ //i = 1 when we reach the EOF
		lb = bl_gen_en(bl, in);
		
		SHOWPRINT(text, bl);

		efunc(bl, keys);

		SHOWPRINT(encrypted, bl);

		for (j = 0; j < 16 ; j++){
			c = bl[j];
			fputc(c,out);
		}

	}
	fclose(out);


	return 0;
}

int OFB_mode_en(char *name, uint8_t keys[19][16], uint8_t *sync)//outtext feedback mode encryption
{
	uint8_t gamma[16];
	uint8_t bl[16];
	uint8_t R[32];
	int j, c, lb = 0;//for last block
	FILE *out, *in;

	out = fopen("encrypted", "w");
	in = fopen(name, "r");

		if (!in){
		
			printf ("can't open en-/decrypting file \"%s\"\n", name);
			return 1;
		}

	memcpy(R, sync, 32); //initialisation R by synchro

	while (!lb){

		lb = bl_gen_en(bl, in);

		memcpy(gamma, R, 16);
		
		SHOWPRINT(text, bl);

		efunc(gamma, keys);
		Xfunc(bl, gamma);

		SHOWPRINT(encrypted, bl);
		
		for (j = 0; j < 16 ; j++){
			c = bl[j];
			fputc(c,out);
		}


/*		printf("R in\n");
		for (j = 0; j < 32 ; j++)
			printf("%X  ", R[j]);
		printf("\n");
*/
		memmove(R, R + 16, 16);
		memcpy(R + 16, gamma, 16);

/*		printf("R out\n");
		for (j = 0; j < 32 ; j++)
			printf("%X  ", R[j]);
		printf("\n");*/
	}

	return 0;
}

int simple_mode_de(uint8_t keys[10][16], char *name)
{
	int lb = 0, j, y = 0, c;
	uint8_t bl[16], bl_next[16];
	FILE *out;

	out = fopen("decrypted", "w");
	lb = bl_gen_de(bl_next, name);

	while (!lb){
		memcpy(bl, bl_next, 16);
		lb = bl_gen_de(bl_next, name);

		SHOWPRINT(text, bl);

		dfunc(bl, keys);

		SHOWPRINT(decrypted, bl);

		if (!lb){
			for (j = 0; j < 16 ; j++){
				c = bl[j];
				fputc(c,out);
			}
		} else {	
			j = 16;
			while (bl[j] != 0x01)
				j--;
			j--;

			while (y <= j){
				c = bl[y];
				fputc(c,out);
				y++;
			}
		}
			
	}
	fclose(out);


	return 0;
}

int OFB_mode_de(char *name, uint8_t keys[10][16], uint8_t *sync)//outtext feedback mode encryption
{
	uint8_t gamma[16];
	uint8_t bl[16];
	uint8_t R[32];
	int j, c, lb = 0;
	FILE *out;

	out = fopen("decrypted", "w");
	memcpy(R, sync, 32);//initialising by synchro

	while (!lb){

		lb = bl_gen_de(bl, name);

		memcpy(gamma, R, 16);
		
		SHOWPRINT(text, bl);

		efunc(gamma, keys);
		Xfunc(bl, gamma);

		SHOWPRINT(decrypted, bl);

		for (j = 0; j < 16 ; j++){
			c = bl[j];
			fputc(c,out);
		}


/*		printf("R\n");
		for (j = 0; j < 32 ; j++)
			printf("%X  ", R[j]);
		printf("\n\n");
*/
		memmove(R, R + 16, 16);

		memcpy(R + 16, gamma, 16);

/*		printf("R\n");
		for (j = 0; j < 32 ; j++)
			printf("%X  ", R[j]);
		printf("\n\n");*/
	}

	return 0;
}


int main(int argc, char *argv[])
{
	int i;
	uint8_t keys[10][16];
	uint8_t sync[32];
	char c ;
	char *kf = 0, *tf = 0, *sf = 0;
	char d = 0;//for direction: 0-encryption 1-decryption as default - encryotion
	char m = 0;//for mode: 0-simple mode 1-Outtext Feedback(OFB) as default - simple mode.

	while (--argc > 0) {
		if ((*++argv)[0] == '-'){
			while (c = *++argv[0]){
				switch (c){
					case 'e': //for encryption
						d = 0;
						break;
					case 'd': //for decryption
						d = 1;
						break;
					case 's': //for simple mode
						m = 0;
						break;
					case 't': //for OFB mode
						m = 1;
						break;
				}
			}
		} else {
			if (!kf)
				kf = *argv;

			else if (!tf)
				tf = *argv;

			else if (!sf && m)
				sf = *argv;
			else
				printf("too many arguments");
			}
		


	}
	
	if (!kf || !tf || (!sf && m)){
		printf("Usage: kuzn -mode,direction keyfile textfile syncfile `\n");
		return 1;
	}
						

	gf_tables_init(0xC3);
	keys_gen(keys, kf);

	for (i = 0; i < 10 ; i++){
		SHOWPRINT(key, keys[i]);
	}
	
	switch(d){
		case 0:
			switch(m){
				case 0:
					simple_mode_en(keys, tf);
					break;

				case 1:
					sync_gen(sync, sf);
					OFB_mode_en(tf, keys, sync);
					break;
			}
			break;

		case 1:
			switch(m){
				case 0:
					simple_mode_de(keys, tf);
					break;

				case 1:
					sync_gen(sync, sf);
					OFB_mode_de(tf, keys, sync);
					break;
			}
			break;
	}
			
			
	
	
	return 0;
}
