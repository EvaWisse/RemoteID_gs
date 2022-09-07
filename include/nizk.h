#ifndef NIZK_H
#define NIZK_H

#include "miracl.h"
#include "utils.h"

typedef struct nizk_proof_struct
{
  BIG c;
  BIG r;
} nizk_proof_struct;

int nizk_verify(ECP A, ECP G, nizk_proof_struct proof);
void nizk_prove(ECP A, BIG a, ECP G, nizk_proof_struct *proof);

#endif