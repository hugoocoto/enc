#ifndef B64_H_
#define B64_H_

#include <stddef.h>

char *b64_enc(const void *data, size_t size, size_t *new_size);
void *b64_dec(const char *data, size_t size, size_t *new_size);

#endif // !64_H_

#ifndef COMPILE_TIME
#define INCLUDE_B64_IMPLEMENTATION // TODO: REMOVE THIS LINE
#endif

#ifdef INCLUDE_B64_IMPLEMENTATION

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

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
const char b64_dec_lookup[] = {
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
        ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63
};
// clang-format on

char *
b64_enc(const void *data, size_t size, size_t *new_size)
{
        const unsigned char *d = (const unsigned char *) data;

        *new_size = (size / 3 + (size % 3 != 0)) * 4;
        char *output = (char *) malloc(*new_size + 1);
        size -= 2;

#pragma omp parallel for
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

        switch (size % 3) {
        case 0: break;

        case 1: {
                unsigned int i = (d[size - 1] & 0xFC) >> 2;
                unsigned int j = ((d[size - 1] & 0x03) << 4);
                output[*new_size - 4] = b64_enc_lookup[i];
                output[*new_size - 3] = b64_enc_lookup[j];
                output[*new_size - 2] = '=';
                output[*new_size - 1] = '=';
        } break;

        case 2: {
                unsigned int i = (d[size - 2] & 0xFC) >> 2;
                unsigned int j = ((d[size - 2] & 0x03) << 4) | ((d[size - 1] & 0xF0) >> 4);
                unsigned int k = ((d[size - 1] & 0x0F) << 2);
                output[*new_size - 4] = b64_enc_lookup[i];
                output[*new_size - 3] = b64_enc_lookup[j];
                output[*new_size - 2] = b64_enc_lookup[k];
                output[*new_size - 1] = '=';
        } break;

        default:
                assert("unreachable" && 0);
                break;
        }

        output[*new_size] = 0;
        return output;
}

void *
b64_dec(const char *data, size_t size, size_t *new_size)
{
        (void) data;
        (void) size;
        (void) new_size;
        return NULL;
}


#endif // !INCLUDE_B64_IMPLEMENTATION
