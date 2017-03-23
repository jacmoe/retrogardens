#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <string.h>

const NUMBER_OF_DEGREES = 4096;			// degrees in a circle
const SHIFT = 16;						// fixed point shift
const long SHIFT_MULT = (1L << SHIFT);	// fixed point shift as a multiplication

void main()
{
	printf("\nGenerating trig.h and trig.cpp fixed point math tables.\n\n");

	double radians = 0.0;
	FILE* fname;
	char hex[20];

	// create file trig.h for constants and macros:
	fname = fopen("trig.h", "wt");

	// macro definitions for trig functions:
	fprintf(fname, "#define COS(X) cos_table[X & (NUMBER_OF_DEGREES - 1)]\n");
	fprintf(fname, "#define SIN(X) sin_table[X & (NUMBER_OF_DEGREES - 1)]\n");

	// constant definitions:
	fprintf(fname, "\n\nconst NUMBER_OF_DEGREES = %i;\n", NUMBER_OF_DEGREES);
	fprintf(fname, "const SHIFT = %d;\n", SHIFT);
	fprintf(fname, "const SHIFT_MULT = 1 << SHIFT;\n\n");

	// external table declarations:
	fprintf(fname, "extern long cos_table[%d];\n", NUMBER_OF_DEGREES);
	fprintf(fname, "extern long sin_table[%d];\n", NUMBER_OF_DEGREES);

	fclose(fname);

	// create file trig.cpp for data tables:
	fname = fopen("trig.cpp", "wt");
	fprintf(fname, "\n//TRIG.CPP\n");
	fprintf(fname, "//\tComputer generated fixed point math tables\n\n");

	// create cosine table:
	fprintf(fname, "long cos_table[%d] = {\n\t", NUMBER_OF_DEGREES);
	int count = 0;

	printf("Cosine table");
	// loop through 4096 degrees:
	for(int i = 0; i < NUMBER_OF_DEGREES; i++)
	{
		if(i % 100 == 0 ) printf(".");
		// translate degrees into radians:
		radians = (double)i / NUMBER_OF_DEGREES * 3.14159 * 2;

		// get cosine:
		long temp_long = (long)(cos(radians) * SHIFT_MULT);

		// convert cosine to hexadecimal ASCII:
		itoa(temp_long, hex, 16);

		// print it to file:
		fprintf(fname, "0x%s, ", hex);

		// output carriage return if eighth entry printed:
		count++;
		if( count >= 8)
		{
			fprintf(fname, "\n\t");
			count = 0;
		}
	}
	fprintf(fname, "};\n\n");

	printf("\n\nSine table");
	// create sine table:
	radians = 0.0;
	fprintf(fname, "long sin_table[%d] = {\n\t", NUMBER_OF_DEGREES);
	count = 0;

	// loop through 4096 degrees:
	for(i = 0; i < NUMBER_OF_DEGREES; i++)
	{
		if(i % 100 == 0 ) printf(".");
		// translate degrees into radians:
		radians = (double)i / NUMBER_OF_DEGREES * 3.14159 * 2;

		// get sine:
		long temp_long = (long)(sin(radians) * SHIFT_MULT);

		// convert cosine to hexadecimal ASCII:
		itoa(temp_long, hex, 16);

		// print it to file:
		fprintf(fname, "0x%s, ", hex);

		// output carriage return if eighth entry printed:
		count++;
		if( count >= 8)
		{
			fprintf(fname, "\n\t");
			count = 0;
		}
	}
	fprintf(fname, "};\n\n");

	fclose(fname);

	printf("\n\nAll done!\n\n");
}
