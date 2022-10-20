#ifndef CPA_SIGN_H_
#define CPA_SIGN_H_

#include "../include/miracl.h"
#include "../include/utils.h"

BIG p, big;
ECP Z, Y, m1, m2, P, ecp;
ECP2 Y_hat, P_hat, ecp2;
char bc[4 * ecp_size + ecp2_size + m_size + 2 * big_size];
csprng RNG;
char m[32];
hash256 sh256;

void sign();
#endif
