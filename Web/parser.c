#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *HTMLFile, *outFile, *CSSFile, *jsFile;
    char c;

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

    if ((jsFile = fopen("main.js", "r")) == NULL)
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

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    fprintf(outFile, "#ifndef _WEBCONFIG_H\n");
    fprintf(outFile, "#define _WEBCONFIG_H\n\n");

    fprintf(outFile, "String webHTML = \"");
    do
    {

        c = fgetc(HTMLFile);

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

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    // CSS FILE
    fprintf(outFile, "String webCSS = \"");
    do
    {

        c = fgetc(CSSFile);

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

    fprintf(outFile, "String webJs = \"");
    do
    {

        c = fgetc(jsFile);

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

    fputc('\n', outFile);

    fprintf(outFile, "\n#endif\n");

    fclose(jsFile);
    fclose(outFile);
    return 0;
}
