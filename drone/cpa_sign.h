#ifndef CPA_SIGN_H_
#define CPA_SIGN_H_

#include "../include/miracl.h"
#include "../include/utils.h"

BIG p;
ECP Z, Y, m1, m2, P;
ECP2 Y_hat, P_hat;
char bc[4 * ecp_size + ecp2_size + m_size + 2 * big_size];

void sign();
#endif
