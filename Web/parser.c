#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *HTMLFile, *outFile, *CSSFile;
    char c;
    int index = 0;

    if ((HTMLFile = fopen("index.html", "r")) == NULL)
    {
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    if ((CSSFile = fopen("styles.css", "r")) == NULL)
    {
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    if ((outFile = fopen("../Tiva/main/webConfig.h", "w")) == NULL)
    {
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    fprintf(outFile, "#ifndef _WEBCONFIG_H\n");
    fprintf(outFile, "#define _WEBCONFIG_H\n\n");

    fprintf(outFile, "String webHTML = \"");
    do
    {

        c = fgetc(HTMLFile);
        if (index == 18)
            printf("%d", c);
        index++;

        if (c != 13 && c != 10 && c != 11 && c != EOF)
        {
            if (c == '"')
            {
                fputc('\\', outFile);
                fputc('\"', outFile);
            }
            else
            {
                fputc(c, outFile);
            }
        }
    } while (c != EOF);

    fprintf(outFile, "\";\n");
    fclose(HTMLFile);

    // CSS FILE
    fprintf(outFile, "String webCSS = \"");
    do
    {

        c = fgetc(CSSFile);
        if (index == 18)
            printf("%d", c);
        index++;

        if (c != 13 && c != 10 && c != 11 && c != EOF)
        {
            if (c == '"')
            {
                fputc('\\', outFile);
                fputc('\"', outFile);
            }
            else
            {
                fputc(c, outFile);
            }
        }
    } while (c != EOF);

    fprintf(outFile, "\";");
    fprintf(outFile, "\n\n#endif\n");
    fclose(CSSFile);
    fclose(outFile);
    return 0;
}
