#include "Base64.h"

#include <string.h>

static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int is_base64_char(const char c)
{
    if (c <= 0)
    {
        return 0;
    }
    return strchr(base64, c) != NULL || c == '=';
}

int is_base64_string(const char *str, const unsigned int len)
{
    if (str == NULL || len == 0 || (len % 4) != 0)
    {
        return 0;
    }
    
    char *p = (char *)str;
    while (p - str < len)
    {
        if (!is_base64_char(*p))
        {
            return 0;
        }
        p++;
    }
    return 1;
}

int base64_encode(const unsigned char *bytes, const unsigned int len, char *out)
{
    if (bytes == NULL || len == 0)
    {
        return -1;
    }

    int new_len = len;
    while (new_len % 4)
    {
        new_len++;
    }
    new_len = (new_len / 3) << 2;

    if (out == NULL)
    {
        return new_len;
    }

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

    switch (((new_len >> 2) * 3) - len)
    {
    case 2:
        out[new_len - 2] = '=';

    case 1:
        out[new_len - 1] = '=';
    }
    return new_len;
}


int base64_decode(const char *str, const unsigned int len, unsigned char *out)
{
    if (str == NULL || len == 0)
    {
        return -1;
    }

    if (is_base64_string(str, len) == 0)
    {
        return -2;
    }

    int new_len = ((len >> 2) * 3) - (str[len - 1] == '=' ? 1 : 0) - (str[len - 2] == '=' ? 1 : 0);

    if (out == NULL)
    {
        return new_len;
    }

    unsigned int i1 = 0, i2 = 0, j;
    char from[4];
    unsigned char to[3];
    char *p = NULL;

    while (i1 < len)
    {
        memcpy(&from[0], &str[i1], 4);
        i1 += 4;

        for (j = 0; j < 4; j++)
        {
            p = strchr(base64, from[j]);
            from[j] = !p ? '\0' : (p - base64);
        }

        to[0] = (from[0] << 2) | (from[1] >> 4);
        to[1] = (from[1] << 4) | (from[2] >> 2);
        to[2] = (from[2] << 6) | from[3];

        if (i2 + 3 < new_len)
            memcpy(&out[i2], to, 3);
        else
            memcpy(&out[i2], to, new_len - i2);
        i2 += 3;
    }
    return new_len;
}
