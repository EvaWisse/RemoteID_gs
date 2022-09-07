#include "../include/pke.h"

int pke_setup(pke_S_struct *S, pke_W_struct *W, pke_param_struct *param, csprng RNG)
{
  param->P1.len = 3;
  param->P1.val[1] = 0x1;
  param->P1.val[2] = 0x2;
  param->P2.len = 4;
  param->P2.val[0] = 0x0;
  param->P2.val[1] = 0x1;
  param->P2.val[2] = 0x2;
  param->P2.val[3] = 0x3;
  if(ECP_KEY_PAIR_GENERATE(&RNG, &S->S, &W->W))
  {
    printf("\tPKE ERROR, key pair could not be generated\n");
    return EXIT_FAILURE;
  }
  if(ECP_PUBLIC_KEY_VALIDATE(&W->W))
  {
    printf("\tPKE ERROR, ECP Public Key is invalid!\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
} 

void pke_encryption(pke_param_struct param, octet W, pke_cipher_struct *cipher, octet M, csprng RNG)
{
  ECP_ECIES_ENCRYPT(HASH_TYPE_BN254, &param.P1, &param.P2, &RNG, &W, &M, 12, &cipher->V, &cipher->C, &cipher->T);
}

int pke_decryption(pke_param_struct param, octet U, pke_cipher_struct cipher, octet *M)
{
  if(!ECP_ECIES_DECRYPT(HASH_TYPE_BN254, &param.P1, &param.P2, &cipher.V, &cipher.C, &cipher.T, &U, M))
  {
    printf("\tPKE ERROR, ECIES Decryption Failed.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}