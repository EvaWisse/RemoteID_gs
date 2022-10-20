#ifndef MIRACL_H
#define MIRACL_H

#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include "miracl/pair_BN254.h"
#include "miracl/randapi.h"

using namespace core;
using namespace B256_28;
using namespace BN254; 

#define ecp_size 66
#define ecp2_size 129
#define big_size 32
#define m_size 32

typedef struct message_struct
{
  char uav_loc[12];
  char gs_loc[12];
  char uav_v[6];
  char ts[4];
  char es; 
} message_struct;

#endif