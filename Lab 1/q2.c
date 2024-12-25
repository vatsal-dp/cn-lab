#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage(void)
{
    printf("Usage:\n");
    printf(" -i <infile>\n");
    printf(" -o <outfile>\n");
    exit(8);
}

int main(int argc, char **argv)
{
    /* Pointer to the source file */
    FILE *src;
    FILE *dest;
    /* File is read one character at a time*/
    char c;

    char x;
    char *st = "sample.txt";
    char *dt = "sample_out.txt";
    while ((x = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (x)
        {
        case 'i':
            st = optarg;
            break;
        case 'o':
            dt = optarg;
            break;
        default:
            usage();
            exit(1);
        }
    }

    src = fopen(st, "r");
    dest = fopen(dt, "w");
    if (src == NULL)
    {
        printf("File not found. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    /* Read src until end-of-file char is encountered */
    while ((c = fgetc(src)) != EOF)
    {
        printf("%c", c);
        fputc(c, dest);
    }
    fclose(src);
    return 0;
}