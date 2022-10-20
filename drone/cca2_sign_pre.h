#ifndef CCA2_SIGN_PRE_H_
#define CCA2_SIGN_PRE_H_

#include "../include/miracl.h"
#include "../include/utils.h"

BIG p;
ECP Z, Y, m1, m2, P;
ECP2 Y_hat, P_hat;
char bc[4 * ecp_size + 3 * ecp2_size + m_size + 3 * big_size];

void sign();
#endif
