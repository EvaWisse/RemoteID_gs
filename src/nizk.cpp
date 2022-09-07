#include "../include/nizk.h"

void nizk_prove(ECP A, BIG a, ECP G, nizk_proof_struct *proof)
{
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG, &RAW);

  BIG p, v; 
  BIG_rcopy(p, CURVE_Order);  
  BIG_randtrunc(v, p, 2 * CURVE_SECURITY_BN254, &RNG);

  ECP V;
  ECP_copy(&V, &G);
  ECP_mul(&V, v); // V = vG

  char digest[32];
  hash256 sh256;
  HASH256_init(&sh256);
  
  hash_ECP(&sh256, G);
  hash_ECP(&sh256, V);
  hash_ECP(&sh256, A);
  
  HASH256_hash(&sh256, digest);
  BIG_fromBytesLen(proof->c, digest, 32);
  BIG_mod(proof->c, p);

  BIG temp;
  BIG_modmul(proof->r, a, proof->c, p);  // ac
  BIG_modneg(temp, proof->r, p); // -ac
  BIG_modadd(proof->r, v, temp, p); // v + (-ac)
}

int nizk_verify(ECP A, ECP G, nizk_proof_struct proof)
{
  ECP V, T;
  ECP_copy(&V, &G);
  ECP_mul(&V, proof.r); // rG
  ECP_copy(&T, &A);
  ECP_mul(&T, proof.c); // cA
  ECP_add(&V, &T); // rG + cA

  BIG c, p;
  char digest[32];
  hash256 sh256;
  HASH256_init(&sh256);
  
  hash_ECP(&sh256, G);
  hash_ECP(&sh256, V);
  hash_ECP(&sh256, A);
  
  HASH256_hash(&sh256, digest);
  BIG_fromBytesLen(c, digest, 32);
  BIG_rcopy(p, CURVE_Order);  
  BIG_mod(c, p);

  if(BIG_comp(c, proof.c))
  {
    printf("\tERROR NIZK hash does not verify.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}