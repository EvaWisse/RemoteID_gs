#ifndef CPA_DESKTOP_H_
#define CPA_DESKTOP_H_

#include "../include/miracl.h"
#include "../include/ds.h"
#include "../include/nizk.h"
#include "../include/pke.h"
#include "../include/spseq.h"
#include "../include/utils.h"

typedef struct reg_struct
{
  pke_cipher_struct C;
  ds_sig_struct     sig;
} reg_struct;

typedef struct ttp_struct
{
  BIG           spseq_sk[2];
  pke_S_struct  pke_sk;
  reg_struct    reg;
} ttp_struct;

typedef struct drone_struct
{
  ECP2              spseq_pk[2];
  spseq_sig_struct  sig;
  ECP               m1;
  ECP               m2;
} drone_struct;

typedef struct drone_setup_struct
{
  BIG         q;
  ds_S_struct ds_sk;
} drone_setup_struct;

typedef struct shared_setup_struct
{
  ECP                 U;
  ECP                 Q;
  pke_cipher_struct   pke_cipher;
  ds_sig_struct       ds_sig;
  ds_W_struct         ds_pk;
  nizk_proof_struct   nizk_proof[2];
  spseq_sig_struct    spseq_sig;
} shared_setup_struct;

typedef struct drone_output_stuct
{
  ECP m1;
  ECP m2;
  spseq_sig_struct new_sig;
  message_struct m;
  BIG z1;
  BIG z2;
  ECP2 C1;
  ECP2 C2;
  BIG c;
} drone_output_stuct;

typedef struct shared_struct
{
  ECP2              spseq_pk[2];
  pke_W_struct      pke_pk;
  bg_struct         spseq_bg;
  pke_param_struct  pke_param;
} shared_struct;

shared_setup_struct shared_setup;
shared_struct shared;
drone_setup_struct drone_setup;
drone_struct drone;
ttp_struct ttp;
drone_output_stuct drone_output;

int setup();
int join();
int join_part1();
int join_ttp();
int join_part2();
int sign();
int open();
int verify();

#endif
