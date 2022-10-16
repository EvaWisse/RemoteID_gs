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

#define ecp_size 64
#define ecp2_size 128
#define big_size 32
#define m_size 32

void ECP2_toChar(char W[], ECP2 *P);
void ECP_toChar(char W[], ECP *P);
int dec_fromHex(char hex);
void BIG_fromChar(BIG *a, char ch[]);

#endif
