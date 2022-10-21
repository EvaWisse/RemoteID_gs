#ifndef CPA_SIGN_PRE_H_
#define CPA_SIGN_PRE_H_

#include "../include/miracl.h"
#include "../include/utils.h"

BIG p, big;
ECP Z, Y, m1, m2, P, ecp;
ECP2 Y_hat, P_hat, ecp2;
char bc[4 * ecp_size + ecp2_size + m_size + 2 * big_size];
int rho_index, v_index, y_index;
hash256 sh256;
char m[32];
byte i;
void sign();
#endif
