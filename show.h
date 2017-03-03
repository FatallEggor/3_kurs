#ifndef SHOWMODE
	#define SHOWMODE	0
#endif

#if SHOWMODE == 1
	#define GENERATOR(file, c)	(fscanf(file, "%2x", &c)) == EOF
#else
	#define GENERATOR(file, c)	(c = fgetc(file))== EOF
#endif

#if SHOWMODE == 1
	#define SHOWPRINT(text, m)\
	do {\
		fprintf (stderr, #text "\n"); \
		int j;\
		for(j = 0; j < 16; j++)\
			fprintf (stderr, "%2X  ", m[j]);\
		fprintf(stderr, "\n\n");\
	} while (0)
#endif

#ifndef SHOWPRINT
	#define SHOWPRINT(arg...)
#endif
