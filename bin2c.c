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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#ifndef __WIN32
#define O_BINARY 0
#endif

int
main ( int argc, char* argv[] )
{
    unsigned char buf[BUFSIZ];
    char* ident;
    int fd, i, total, rd, need_comma;

    FILE *f_output;

    if ( argc < 4 )
    {
        fprintf ( stderr, "Usage: %s binary_file output_file array_name\n", argv[0] );
        return -1;
    }

    fd = open ( argv[1], O_BINARY | O_RDONLY );
    if ( fd == -1 )
    {
        fprintf ( stderr, "%s: can't open %s for reading\n", argv[0], argv[1] );
        return -1;
    }

    f_output = fopen(argv[2], "w");
    if ( f_output == NULL )
    {
        fprintf ( stderr, "%s: can't open %s for writing\n", argv[0], argv[1] );
        return -1;
    }

    ident = argv[3];

    fprintf ( f_output, "const char %s[] = {", ident );
    for ( total = 0, need_comma = 0; ( rd = read ( fd, buf, BUFSIZ ) ) != 0; )
    {
        if ( rd == -1 )
        {
            fprintf ( stderr, "%s: file read error\n", argv[0] );
            return -1;
        }
        for ( i = 0; i < rd; i++ )
        {
            if ( need_comma ) fprintf ( f_output, ", " );
            else need_comma = 1;
            if ( ( total % 11 ) == 0 ) fprintf ( f_output, "\n\t" );
            fprintf ( f_output, "0x%.2x", buf[i] );
            total++;
        }
    }
    fprintf ( f_output, "\n};\n\n" );

    fprintf ( f_output, "const int %s_length = %i;\n", ident, total );

    close ( fd );
    fclose ( f_output );

    return 0;
}
