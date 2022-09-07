
#ifndef H_INC_H
#define H_INC_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "big_B256_28.h"
#include "config_big_B256_28.h"
#include "ecp_BN254.h"
#include "ecp2_BN254.h"
#include "fp_BN254.h"
#include "fp2_BN254.h"
#include "randapi.h"

using namespace core;
using namespace B256_28;
using namespace BN254;

#define rounds 51

void ECP2_toChar(char W[], ECP2 *P);
void ECP_toChar(char W[], ECP *P);


// // remove pn drone
// void FP2_fromBytes(FP2 *x,char *b);
// int ECP2_equals(ECP2 *P, ECP2 *Q);
// int ECP_equals(ECP *P, ECP *Q);
// void FP_fromBytes(FP *x,char *b);
// void ECP_toFile(ECP ecp, FILE *fp);
// void ECP2_toFile(ECP2 ecp2, FILE *fp);
// void FP2_toFile(FP2 fp2, FILE *fp);
// void ECP2_fromFile(FILE *fp, ECP2 *ecp2);
// void ECP_fromFile(FILE*fp, ECP *ecp);
// void BIG_toFile(BIG big, FILE *fp);
// void BIG_fromFile(FILE *fp, BIG *a);
// void BIG_fromChar(BIG *a, char ch[]);
// void ECP2_fromChar(ECP2 *ecp2, char ch[]);
// void ECP_fromChar(ECP *ecp, char ch[]);
// int dec_fromHex(char hex);


#endif