#ifndef CCA2_SIGN_PRE_H_
#define CCA2_SIGN_PRE_H_

#include "../include/miracl.h"
#include "../include/utils.h"

BIG p, big;
ECP Z, Y, m1, m2, P, ecp;
ECP2 Y_hat, P_hat, ecp2;
int rho_index, u_index, v_index, n_index, y_index;
hash256 sh256;
char bc[4 * ecp_size + 3 * ecp2_size + m_size + 3 * big_size];
char m[32];
csprng RNG;

void sign();
#endif
