/* * Copyright (c) 2026 Hugo Coto Florez
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/
 *
 * SPDX-License-Identifier: CC-BY-4.0
 */

/* Usage
 *
 * It's needed to include the implementation in one and only one file. From
 * other files the library can be used by including the heading and calling the
 * functions as expected.
 *
 * #define INCLUDE_B64_IMPLEMENTATION
 * #include "base64.h"
 *
 * This library uses OpenMP for parallelism, it is needed to compile with the
 * `-fopenmp` flag. If the compiler does not support this feature, the library
 * can be used without modification, running in a single thread.
 */

#ifndef B64_H_
#define B64_H_

#include <stddef.h> // size_t

#define B64_PADDING_CHAR '='

char *b64_enc(const void *data, size_t size, size_t *new_size);
void *b64_dec(const char *data, size_t size, size_t *new_size);

#endif // !64_H_


#ifdef INCLUDE_B64_IMPLEMENTATION

#ifndef B64_MALLOC
/*   */ #include<stdlib.h> // malloc
/*   */ #define B64_MALLOC malloc
#endif // !B64_MALLOC

const char b64_enc_lookup[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
};

// clang-format off
const unsigned char b64_dec_lookup[] = {
        ['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  
        ['E'] = 4,  ['F'] = 5,  ['G'] = 6,  ['H'] = 7,  
        ['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11, 
        ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15, 
        ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, 
        ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, 
        ['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, 
        ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31, 
        ['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, 
        ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39, 
        ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, 
        ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47, 
        ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, 
        ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55, 
        ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59, 
        ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63,
        [B64_PADDING_CHAR] = 0,
};
// clang-format on

char *
b64_enc(const void *data, size_t size, size_t *new_size)
{
        extern const char b64_enc_lookup[];
        const unsigned char *d = (const unsigned char *) data;
        int sm3;
        char *output;

        sm3 = size % 3;
        *new_size = (size / 3 + (sm3 != 0)) * 4;
        output = (char *) B64_MALLOC(*new_size + 1);
        size -= 2;

/*   */ #pragma omp parallel for
        for (size_t di = 0; di < size; di += 3) {
                unsigned int i = (d[di + 0] & 0xFC) >> 2;
                unsigned int j = ((d[di + 0] & 0x03) << 4) | ((d[di + 1] & 0xF0) >> 4);
                unsigned int k = ((d[di + 1] & 0x0F) << 2) | ((d[di + 2] & 0xC0) >> 6);
                unsigned int l = (d[di + 2] & 0x3F);
                char *out = &output[di / 3 * 4];
                out[0] = b64_enc_lookup[i];
                out[1] = b64_enc_lookup[j];
                out[2] = b64_enc_lookup[k];
                out[3] = b64_enc_lookup[l];
        }

        size += 2;

        switch (sm3) {
        case 1: {
                unsigned int i = (d[size - 1] & 0xFC) >> 2;
                unsigned int j = ((d[size - 1] & 0x03) << 4);
                char *out = &output[*new_size];
                out[-4] = b64_enc_lookup[i];
                out[-3] = b64_enc_lookup[j];
                out[-2] = B64_PADDING_CHAR;
                out[-1] = B64_PADDING_CHAR;
        } break;

        case 2: {
                unsigned int i = (d[size - 2] & 0xFC) >> 2;
                unsigned int j = ((d[size - 2] & 0x03) << 4) | ((d[size - 1] & 0xF0) >> 4);
                unsigned int k = ((d[size - 1] & 0x0F) << 2);
                char *out = &output[*new_size];
                out[-4] = b64_enc_lookup[i];
                out[-3] = b64_enc_lookup[j];
                out[-2] = b64_enc_lookup[k];
                out[-1] = B64_PADDING_CHAR;
        } break;
        }

        output[*new_size] = 0;
        return output;
}

void *
b64_dec(const char *data, size_t size, size_t *new_size)
{
        /*
         * Size have to be multiple of 4. If data is the return value of
         * b64_enc it is valid.
         */

        extern const unsigned char b64_dec_lookup[];
        const unsigned char *d = (const unsigned char *) data;
        char *output;

        *new_size = size / 4 * 3 -
                    (d[size - 1] == B64_PADDING_CHAR) -
                    (d[size - 2] == B64_PADDING_CHAR);

        output = (char *) B64_MALLOC(*new_size + 1);

/*   */ #pragma omp parallel for
        for (size_t o = 0; o < size; o += 4) {
                unsigned char d_0 = b64_dec_lookup[d[o + 0]];
                unsigned char d_1 = b64_dec_lookup[d[o + 1]];
                unsigned char d_2 = b64_dec_lookup[d[o + 2]];
                unsigned char d_3 = b64_dec_lookup[d[o + 3]];
                char *out = &output[o / 4 * 3];
                out[0] = ((d_0 & 0x3F) << 2) | ((d_1 & 0x30) >> 4);
                out[1] = ((d_1 & 0x0F) << 4) | ((d_2 & 0x3C) >> 2);
                out[2] = ((d_2 & 0x03) << 6) | ((d_3 & 0x3F));
        }

        output[*new_size] = 0;
        return output;
}

#endif // !INCLUDE_B64_IMPLEMENTATION
