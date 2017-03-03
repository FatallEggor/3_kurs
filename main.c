#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "kuzn.h"
#include "gf.h"
#include "show.h"

#define ENCRYPTION	0x0001
#define DECRYPTION	0x0002
#define SIMPLE_MODE	0x0004
#define OFB_MODE 	0x0008
#define HAS_KEY_FILE	0x0010
#define HAS_INPUT_FILE	0x0020
#define HAS_OUTPUT_FILE	0x0040

#define DEFAULT_OPT	(ENCRYPTION | SIMPLE_MODE)
#define OBLIG_OPTIONS	HAS_KEY_FILE

static const char	*program_name;



static void
set_program_name(int argc, char *argv[])
{
	char *s;

	program_name = ((s = strrchr(argv[0], '/')) != NULL) ? ++s : argv[0];
}
static void
print_usage()
{
	printf("Usage: %s [OPTIONS] [FILE]\n"
	       "Transform standard input or predefined input file by GOST 34.12-15 to standard output or predefined output file.\n"
	       "\n"
	       "Options: \n"
	       "  -e <encrypt>		encrypt input \n"
	       "  -d <decrypt>          decrypt input\n"
	       "  -k <secret-key-file>  secret key file(hex)\n"
	       "  -s <simple mode>      work in simple mode\n"
	       "  -t <OFB mode>      	work in Outtext FeedBack mode with inital gamma synchronisation file (hex)\n"
	       "  -s <sinple mode>      \n"
	       "  -i <input-file>       input file\n"
	       "  -o <output-file>      output file\n"
	       "\n"
	       "  If no FILE provided, then reads standard input. \n"
	       "\n"
	       "Examples: \n"
	       "  %s -e -k key -t sync -i /etc/passwd > passwd.encr\n"
	       "\n",
	       program_name, program_name);
}

int main(int argc, char *argv[])
{
	int i, opt;
	int optflags = DEFAULT_OPT;
	uint8_t keys[10][16];
	uint8_t sync[32];
	char *kf, *inf, *sf, *outf;
	FILE *in, *out;	
	FILE *inkey;
	FILE *insync;

	set_program_name(argc, argv);

	while ((opt = getopt(argc, argv, "edsht:k:o:i:")) != -1){
		switch (opt){
			case 'e': //for encryption
				optflags |= ENCRYPTION;
				break;
			case 'd': //for decryption
				optflags ^= ENCRYPTION;
				optflags |= DECRYPTION;
				break;
			case 's': //for simple mode
				optflags |= SIMPLE_MODE;
				break;
			case 't':
				sf = optarg;
				optflags ^= SIMPLE_MODE;
				optflags |= OFB_MODE;
				break;
			case 'k':
				optflags |= HAS_KEY_FILE;
				kf = optarg;
				break;
			case 'i':
				optflags |= HAS_INPUT_FILE;
				inf = optarg;
				break;
			case 'o':
				optflags |= HAS_OUTPUT_FILE;
				outf = optarg;
				break;
			default:
				print_usage();
				exit(1);
				break;
		}
	}
							
	if ((optflags & OBLIG_OPTIONS) != OBLIG_OPTIONS) {
		printf("%s: missing mandatory options\n", program_name);
		print_usage();
		exit(1);
	}

	gf_tables_init(0xC3);
	
	inkey = fopen(kf, "r"); 
	if (!inkey){
		
		printf ("can't open key-file \"%s\"\n", kf);
		return 1;
	}
	kuzn_keys_gen(keys, inkey);

	for (i = 0; i < 10 ; i++){
		SHOWPRINT(key, keys[i]);//-----------
	}
		
	switch(optflags & (SIMPLE_MODE | OFB_MODE)){
		case SIMPLE_MODE:
		switch(optflags & (ENCRYPTION | DECRYPTION)){
			case ENCRYPTION:			
			if ((optflags & HAS_INPUT_FILE) == HAS_INPUT_FILE){
				in = fopen(inf, "r");
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_simple_mode_en(keys, in, out);
				}else
					kuzn_simple_mode_en(keys, in, stdout);	
			}else
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_simple_mode_en(keys, stdin, out);
				}else
					kuzn_simple_mode_en(keys, stdin, stdout);		
			break;

			case DECRYPTION:
			if ((optflags & HAS_INPUT_FILE) == HAS_INPUT_FILE){
				in = fopen(inf, "r");
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_simple_mode_de(keys, in, out);
				}else
					kuzn_simple_mode_de(keys, in, stdout);	
			}else
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_simple_mode_de(keys, stdin, out);
				}else
					kuzn_simple_mode_de(keys, stdin, stdout);					
			break;
		}
		break;

		case OFB_MODE:

		insync = fopen(sf, "r");	
		if (!insync){
			
			printf ("can't open sync-file \"%s\"\n", sf);
			return 1;
		}
		kuzn_sync_gen(sync, insync);

		switch(optflags & (ENCRYPTION | DECRYPTION)){
			case ENCRYPTION:
			if ((optflags & HAS_INPUT_FILE) == HAS_INPUT_FILE){
				in = fopen(inf, "r");
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_OFB_mode_en(keys, sync, in, out);
				}else
					kuzn_OFB_mode_en(keys, sync, in, stdout);	
			}else
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_OFB_mode_en(keys, sync, stdin, out);
				}else
					kuzn_OFB_mode_en(keys, sync, stdin, stdout);				
			break;

			case DECRYPTION:
			if ((optflags & HAS_INPUT_FILE) == HAS_INPUT_FILE){
				in = fopen(inf, "r");
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_OFB_mode_de(keys, sync, in, out);
				}else
					kuzn_OFB_mode_de(keys, sync, in, stdout);	
			}else
				if ((optflags & HAS_OUTPUT_FILE) == HAS_OUTPUT_FILE){
					out = fopen(outf, "w");
					kuzn_OFB_mode_de(keys, sync, stdin, out);
				}else
					kuzn_OFB_mode_de(keys, sync, stdin, stdout);			
			break;
		}
		break;
	}
			
			
	
	
	return 0;
}
