#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

/*
Return value:
    0 - Is not a Base64.
    1 - Is Base64.
*/
int is_base64_char(
    const char c // Character to check.
);

/*
Return value:
    0 - Is not a Base64 string.
    1 - This is the Base64 string.
*/
int is_base64_string(
    const char *str,        // String for check.
    const unsigned int len  // String length.
);

/*
Return value:
    >0  - Output length in bytes.
    -1  - Invalid data.
*/
int base64_encode(
    const unsigned char *bytes, // Input bytes.
    const unsigned int len,     // Length.
    char *out                   // Output data. If NULL, returns a new length for the output.
);

/*
Return value:
    >0  - Output length in bytes.
    -1  - Invalid data.
    -2  - This is not Base64 string.
*/
int base64_decode(
    const char *str,        // Base64 string.
    const unsigned int len, // Length.
    unsigned char *out      // Output data. If NULL, returns a new length for the output.
);

#endif
