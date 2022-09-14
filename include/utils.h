#ifndef UTILS_H_
#define UTILS_H_

#include "miracl.h"

void hash_ECP(hash256 *ctx, ECP ecp);
void hash_ECP2(hash256 *ctx, ECP2 ecp);

void ECP2_toChar(char W[], ECP2 *P);
void ECP_toChar(char W[], ECP *P);

int dec_fromHex(char hex);
void BIG_fromChar(BIG *a, char ch[]); 
void ECP_fromChar(ECP *ecp, char ch[]);
void ECP2_fromChar(ECP2 *ecp2, char ch[]);

void BIG_toFile(BIG big, FILE *fp);
void FP2_toFile(FP2 fp2, FILE *fp);
void ECP_toFile(ECP ecp, FILE *fp);
void ECP2_toFile(ECP2 ecp2, FILE *fp);
void OCT_toFile(octet oct, FILE *fp);

void BIG_fromFile(FILE *fp, BIG *a);
void ECP_fromFile(FILE*fp, ECP *ecp);
void ECP2_fromFile(FILE *fp, ECP2 *ecp2);
void OCT_fromFile(int *len, char *val, FILE *fp);

#endif