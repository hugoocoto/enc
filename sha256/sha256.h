// https://en.wikipedia.org/wiki/SHA-2

#ifndef SHA256_H_
#define SHA256_H_

#include <stddef.h>

int sha256(int fdin, size_t msg_size, int fdout);

#endif // !SHA256_H_

#ifdef INCLUDE_SHA256_IMPLEMENTATION

#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define rightshift(x, n) ((x) >> (n))
#define rightrotate(x, n) (((x) >> (n)) | (((x) & ((1 << (n)) - 1)) << (sizeof(x) * 8 - (n))))
#define IV { 0 }

int
sha256(int fdin, size_t msg_size, int fdout)
{
        // Initialize hash values:
        uint32_t h0 = 0x6a09e667;
        uint32_t h1 = 0xbb67ae85;
        uint32_t h2 = 0x3c6ef372;
        uint32_t h3 = 0xa54ff53a;
        uint32_t h4 = 0x510e527f;
        uint32_t h5 = 0x9b05688c;
        uint32_t h6 = 0x1f83d9ab;
        uint32_t h7 = 0x5be0cd19;

        // Initialize array of round constants:
        uint32_t k[] = {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        // Pre-processing (Padding):
        size_t pad_len = msg_size % 64;
        size_t pad_zeros = (pad_len < 56) ? (56 - pad_len - 1) : (64 + 56 - pad_len - 1);
        size_t L = msg_size + 1 + pad_zeros + 8;
        uint8_t *data = (uint8_t *) calloc(1, L);
        assert(read(fdin, data, msg_size) > 0);
        data[msg_size] = 0x80;
        uint64_t bit_len = (uint64_t) msg_size * 8;
        uint32_t *len_ptr = (uint32_t *) &data[L - 8];
        len_ptr[0] = htonl((uint32_t)(bit_len >> 32));
        len_ptr[1] = htonl((uint32_t)(bit_len & 0xFFFFFFFF));

        uint32_t w[64] = IV;
        for (size_t c_i = 0; c_i < L; c_i += 64) {
                uint32_t *chunk = (uint32_t *) (data + c_i);

                // copy chunk into first 16 words w[0..15] of the message schedule array
                for (size_t i = 0; i < 16; i++)
                        w[i] = ntohl(chunk[i]);

                // Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array:
                for (size_t i = 16; i < 64; i++) {
                        uint32_t s0 = rightrotate(w[i - 15], 7) ^ rightrotate(w[i - 15], 18) ^ rightshift(w[i - 15], 3);
                        uint32_t s1 = rightrotate(w[i - 2], 17) ^ rightrotate(w[i - 2], 19) ^ rightshift(w[i - 2], 10);
                        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                }

                // Initialize working variables to current hash value:
                uint32_t a = h0;
                uint32_t b = h1;
                uint32_t c = h2;
                uint32_t d = h3;
                uint32_t e = h4;
                uint32_t f = h5;
                uint32_t g = h6;
                uint32_t h = h7;

                //     Compression function main loop:
                for (size_t i = 0; i < 64; i++) {
                        uint32_t S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
                        uint32_t ch = (e & f) ^ ((~e) & g);
                        uint32_t temp1 = h + S1 + ch + k[i] + w[i];
                        uint32_t S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
                        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                        uint32_t temp2 = S0 + maj;

                        h = g;
                        g = f;
                        f = e;
                        e = d + temp1;
                        d = c;
                        c = b;
                        b = a;
                        a = temp1 + temp2;
                }

                // Add the compressed chunk to the current hash value:
                h0 = h0 + a;
                h1 = h1 + b;
                h2 = h2 + c;
                h3 = h3 + d;
                h4 = h4 + e;
                h5 = h5 + f;
                h6 = h6 + g;
                h7 = h7 + h;
        }

        // Produce the final hash value (big-endian)
        uint32_t digest[8] = {
                htonl(h0), htonl(h1), htonl(h2), htonl(h3),
                htonl(h4), htonl(h5), htonl(h6), htonl(h7)
        };
        write(fdout, digest, sizeof digest);

        free(data);
        return 0;
}

#undef rightshift
#undef rightrotate
#undef IV

#endif // !INCLUDE_SHA256_IMPLEMENTATION
