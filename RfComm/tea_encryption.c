#include "tea_encryption.h"

// XXTEA Algorithm modified from https://en.wikipedia.org/wiki/XXTEA

#define DELTA 0x9E3779B9
//#define MX (((z>>5 ^ y<<2) + (y>>3 ^ z<<4)) ^ ((sum ^ y) + (TEA_KEY[(p&3) ^ e] ^ z)))
const uint32_t TEA_KEY[] = {0x4D625165, 0x5468576D, 0x59713374, 0x3677397A};

/*

void btea(uint32_t *v, int n, uint32_t const key[4]) {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    if (n > 1) { // Coding Part 
        rounds = 6 + 52 / n;
        sum = 0;
        z = v[n - 1];
        do {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p = 0; p < n - 1; p++) {
                y = v[p + 1];
                z = v[p] += MX;
            }
            y = v[0];
            z = v[n - 1] += MX;
        } while (--rounds);
    } else if (n < -1) { // Decoding Part 
        n = -n;
        rounds = 6 + 52 / n;
        sum = rounds*DELTA;
        y = v[0];
        do {
            e = (sum >> 2) & 3;
            for (p = n - 1; p > 0; p--) {
                z = v[p - 1];
                y = v[p] -= MX;
            }
            z = v[n - 1];
            y = v[0] -= MX;
            sum -= DELTA;
        } while (--rounds);
    }
}
*/

void encrypt(uint32_t *data) {
    uint32_t y, z, sum;
    uint8_t rounds, e;
    rounds = 32;
    sum = 0;
    z = data[1];
    do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        y = data[1];
        z = data[0] += (((z>>5 ^ y<<2) + (y>>3 ^ z<<4)) ^ ((sum ^ y) + (TEA_KEY[e] ^ z)));
        y = data[0];
        z = data[1] += (((z>>5 ^ y<<2) + (y>>3 ^ z<<4)) ^ ((sum ^ y) + (TEA_KEY[1 ^ e] ^ z)));
    } while (--rounds);
}

void decrypt(uint32_t *data) {
    uint32_t y, z, sum;
    uint8_t rounds, e;
    rounds = 32;
    sum = 0xC6EF3720; // rounds * DELTA
    y = data[0];
    do {
        e = (sum >> 2) & 3;
        z = data[0];
        y = data[1] -= (((z>>5 ^ y<<2) + (y>>3 ^ z<<4)) ^ ((sum ^ y) + (TEA_KEY[1 ^ e] ^ z)));
        z = data[1];
        y = data[0] -= (((z>>5 ^ y<<2) + (y>>3 ^ z<<4)) ^ ((sum ^ y) + (TEA_KEY[e] ^ z)));
        sum -= DELTA;
    } while (--rounds);
}

/*
void decrypt(uint32_t v[2]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720;
    for (i=0; i < 32; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + TEA_KEY[(sum>>11) & 3]);
        sum -= DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + TEA_KEY[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}*/

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */
/*
void encrypt(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += DELTA;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

void decrypt(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=DELTA*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}*/

