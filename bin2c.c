/*
 * This is bin2c program, which allows you to convert binary file to
 * C language array, for use as embedded resource, for instance you can
 * embed graphics or audio file directly into your program.
 * This is public domain software, use it on your own risk.
 * Contact Serge Fukanchik at fuxx@mail.ru  if you have any questions.
 *
 * Some modifications were made by Gwilym Kuiper (kuiper.gwilym@gmail.com)
 * I have decided not to change the licence.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS	16

#ifdef USE_BZ2
#include <bzlib.h>
#endif

int
main(int argc, char *argv[])
{
    unsigned char *buf;
    char array_name[80];
    unsigned int i, file_size, file_count, need_comma;
    int incr = 1;
    int arg = 1;
    const char* filename;

    FILE *f_input, *f_output;

#ifdef USE_BZ2
    char *bz2_buf;
    unsigned int uncompressed_size, bz2_size;
#endif

    if (argc < 4) {
        fprintf(stderr, "Usage: %s [-w] binary_file output_file [array_name]\n",
                argv[0]);
        return -1;
    }

    if (strcmp(argv[arg], "-w") == 0) {
	++arg; incr = 2;
    }

    filename = argv[arg++];
    f_input = fopen(filename, "rb");
    if (f_input == NULL) {
        fprintf(stderr, "%s: can't open %s for reading\n", argv[0], filename);
        return -1;
    }

    // Get the file length
    fseek(f_input, 0, SEEK_END);
    file_size = ftell(f_input);
    fseek(f_input, 0, SEEK_SET);

    buf = (unsigned char *) malloc(file_size);
    assert(buf);

    fread(buf, file_size, 1, f_input);
    fclose(f_input);

#ifdef USE_BZ2
    // allocate for bz2.
    bz2_size =
      (file_size + file_size / 100 + 1) + 600; // as per the documentation

    bz2_buf = (char *) malloc(bz2_size);
    assert(bz2_buf);

    // compress the data
    int status =
      BZ2_bzBuffToBuffCompress(bz2_buf, &bz2_size, buf, file_size, 9, 1, 0);

    if (status != BZ_OK) {
        fprintf(stderr, "Failed to compress data: error %i\n", status);
        return -1;
    }

    // and be very lazy
    free(buf);
    uncompressed_size = file_size;
    file_size = bz2_size;
    buf = bz2_buf;
#endif

    filename = argv[arg++];
    f_output = fopen(filename, "w");
    if (f_output == NULL) {
        fprintf(stderr, "%s: can't open %s for writing\n", argv[0], filename);
        return -1;
    }

	// Check if array_name passed on command line
	if (arg < argc) {
		strcpy(array_name, argv[arg]);
	} else {
		char ch;
		strcpy(array_name, filename);
		// Replace non-alphanumeric chars with underscore
		for (i = 0; (ch = array_name[i]) != '\0'; ++i) {
			if (!isalpha(ch) && !isdigit(ch)) {
				array_name[i] = '_';
			}
		}
			
	}

    need_comma = 0;
	
	file_count = file_size / incr;

	fprintf(f_output, "const unsigned short %s[%i] = {", array_name, file_count);
    for (i = 0; i < file_size; i += incr) {
        if (need_comma)
            fprintf(f_output, ", ");
        else
            need_comma = 1;
        if ((i % COLS) == 0)
            fprintf(f_output, "\n\t");
	if (incr > 1) {
	    fprintf(f_output, "0x%04x", buf[i] | (buf[i + 1] << 8));
	} else {
	    fprintf(f_output, "0x%02x", buf[i]);
	}
    }
    fprintf(f_output, "\n};\n\n");

    fprintf(f_output, "const int %s_length = %i;\n", array_name, file_count);

#ifdef USE_BZ2
    fprintf(f_output, "const int %s_length_uncompressed = %i;\n", array_name,
            uncompressed_size);
#endif

    fclose(f_output);

    return 0;
}
