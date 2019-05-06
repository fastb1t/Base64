#include "base64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char text[] = "Test string";
    char base64[] = "VGVzdCBzdHJpbmc=";

    unsigned int len = 0;
    int new_len = 0;
    char *base64_out = NULL;
    unsigned char *text_out = NULL;


    len = strlen(text);
    new_len = base64_encode((unsigned char *)text, len, NULL);

    if (new_len > 0)
    {
        base64_out = malloc((new_len + 1) * sizeof(char));
        if (base64_out == NULL)
        {
            fprintf(stderr, "Error: failed to allocate memory.\n");
        }
        else
        {
            memset(base64_out, 0, (new_len + 1) * sizeof(char));
            base64_encode((unsigned char *)text, len, base64_out);

            fprintf(stdout, "String: %s\n", text);
            fprintf(stdout, "Length: %d\n", len);
            fprintf(stdout, "New length: %d\n", new_len);
            fprintf(stdout, "Output: %s\n", base64_out);

            free(base64_out);
        }
    }

    fprintf(stdout, "\n");

    len = strlen(base64);
    new_len = base64_decode(base64, len, NULL);

    if (new_len > 0)
    {
        text_out = malloc((new_len + 1) * sizeof(char));
        if (text_out == NULL)
        {
            fprintf(stderr, "Error: failed to allocate memory.\n");
        }
        else
        {
            memset(text_out, 0, (new_len + 1) * sizeof(char));
            base64_decode(base64, len, text_out);

            fprintf(stdout, "String: %s\n", base64);
            fprintf(stdout, "Length: %d\n", len);
            fprintf(stdout, "New length: %d\n", new_len);
            fprintf(stdout, "Output: %s\n", text_out);

            free(text_out);
        }
    }
    return 0;
}
