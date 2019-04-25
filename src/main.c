#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int is_base64_char(const char c)
{
    return isalpha(c) || isdigit(c) || c == '+' || c == '/' || c == '=';
}

int is_base64_string(const char *str, const unsigned int len)
{
    if (!str || !len || (len % 4) != 0)
    {
        return 0;
    }

    char *p = (char *)str;
    while (p - str < len)
    {
        if (!is_base64_char(*p))
            return 0;
        p++;
    }
    return 1;
}

char *base64_encode(const unsigned char *bytes, const unsigned int len, unsigned int *new_len)
{
    if (!bytes || !len || !new_len)
    {
        return 0;
    }

    (*new_len) = len;
    while ((*new_len) % 4)
    {
        (*new_len)++;
    }
    (*new_len) = ((*new_len) / 3) << 2;

    char *out = malloc(((*new_len) + 1) * sizeof(char));
    if (!out)
    {
        (*new_len) = 0;
        return 0;
    }
    memset(out, 0, ((*new_len) + 1) * sizeof(char));

    unsigned int i1 = 0, i2 = 0;
    unsigned char from[3];
    char to[4];

    while (i1 < len)
    {
        from[0] = i1 < len ? bytes[i1] : 0;
        from[1] = (i1 + 1) < len ? bytes[i1 + 1] : 0;
        from[2] = (i1 + 2) < len ? bytes[i1 + 2] : 0;
        i1 += 3;

        to[0] = base64[from[0] >> 2];
        to[1] = base64[((from[0] & 0x03) << 4) | (from[1] >> 4)];
        to[2] = base64[((from[1] & 0x0F) << 2) | (from[2] >> 6)];
        to[3] = base64[from[2] & 0x3F];

        memcpy(&out[i2], to, 4);
        i2 += 4;
    }

    switch ((((*new_len) >> 2) * 3) - len)
    {
    case 2:
        out[(*new_len) - 2] = '=';

    case 1:
        out[(*new_len) - 1] = '=';
    }
    return out;
}

unsigned char *base64_decode(const char *str, const unsigned int len, unsigned int *new_len)
{
    if (!str || !len || !new_len || !is_base64_string(str, len))
    {
        return 0;
    }

    (*new_len) = ((len >> 2) * 3) - (str[len - 1] == '=' ? 1 : 0) - (str[len - 2] == '=' ? 1 : 0);

    unsigned char *out = malloc(((*new_len) + 1) * sizeof (char));
    if (!out)
    {
        (*new_len) = 0;
        return 0;
    }
    memset(out, 0, ((*new_len) + 1) * sizeof(char));

    unsigned int i1 = 0, i2 = 0, j;
    char from[4];
    unsigned char to[3];

    while (i1 < len)
    {
        memcpy(&from[0], &str[i1], 4);
        i1 += 4;

        for (j = 0; j < 4; j++)
        {
            char *p = strchr(base64, from[j]);
            from[j] = !p ? '\0' : (p - base64);
        }

        to[0] = (from[0] << 2) | (from[1] >> 4);
        to[1] = (from[1] << 4) | (from[2] >> 2);
        to[2] = (from[2] << 6) | from[3];

        if (i2 + 3 < (*new_len))
            memcpy(&out[i2], to, 3);
        else
            memcpy(&out[i2], to, (*new_len) - i2);
        i2 += 3;
    }
    return out;
}

int main(int argc, char *argv[])
{
    char plain_text[] =
        "Man is distinguished, not only by his reason, but by this singular passion from other animal"
        "s, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
        "generation of knowledge, exceeds the short vehemence of any carnal pleasure.";

    char base64[] =
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0"
        "aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1"
        "c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0"
        "aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdl"
        "LCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

    unsigned int len;
    unsigned int new_len;
    char *base64_out = NULL;
    unsigned char *plain_text_out = NULL;


    len = strlen(plain_text);
    new_len = 0;
    base64_out = base64_encode((unsigned char *)plain_text, len, &new_len);

    fprintf(stdout, "String: \n%s\n\n", plain_text);
    fprintf(stdout, "Length: %d\n", len);
    fprintf(stdout, "New length: %d\n\n", new_len);

    if (base64_out)
    {
        fprintf(stdout, "Output: \n%s\n", base64_out);
        free(base64_out);
    }
    else
    {
        fprintf(stderr, "Error: failed to allocate memory.\n");
    }


    fprintf(stdout, "\n");


    len = strlen(base64);
    new_len = 0;
    plain_text_out = base64_decode(base64, len, &new_len);

    fprintf(stdout, "Base64: \n%s\n\n", base64);
    fprintf(stdout, "Length: %d\n", len);
    fprintf(stdout, "New length: %d\n\n", new_len);

    if (plain_text_out)
    {
        fprintf(stdout, "Output: \n%s\n", plain_text_out);
        free(plain_text_out);
    }
    else
    {
        fprintf(stderr, "Error: failed to allocate memory.\n");
    }
    return 0;
}
