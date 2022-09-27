#ifndef H_INC_H
#define H_INC_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

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

#define ecp_size 66
#define ecp2_size 129
#define big_size 32

void ECP2_toChar(char W[], ECP2 *P);
void ECP_toChar(char W[], ECP *P);
int dec_fromHex(char hex);
void BIG_fromChar(BIG *a, char ch[]);

// void FP2_fromBytes(FP2 *x, char *b);
// void FP_fromBytes(FP *x, char *b);
#endif