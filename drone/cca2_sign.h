#ifndef CCA2_SIGN_H_
#define CCA2_SIGN_H_

#include "../include/miracl.h"
#include "../include/utils.h"

BIG p, big;
ECP Z, Y, m1, m2, P, ecp;
ECP2 Y_hat, P_hat, ecp2;
hash256 sh256;
char m[32];
csprng RNG;

char bc[4 * ecp_size + 3 * ecp2_size + m_size + 3 * big_size];

void sign();
#endif
