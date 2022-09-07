#ifndef PKE_H
#define PKE_H

#include "miracl.h"
#include "../include/miracl/ecdh_BN254.h"
#include "utils.h"

/**
 * @brief Elliptic Curve Integrated Encryption Scheme (ECIES)
 * 
 */
typedef struct pke_S_struct
{ 
  char s[EGS_BN254];
  octet S = {0, sizeof(s), s};
} pke_S_struct;

typedef struct pke_W_struct
{
  char w[2 * EGS_BN254 + 1];
  octet W = {0, sizeof(w), w};
} pke_W_struct;

/* PKE specific data types */
typedef struct pke_param
{ 
  char p1[30];
  char p2[30];
  octet P1 = {0, sizeof(p1), p1};
  octet P2 = {0, sizeof(p2), p2};
} pke_param_struct;

typedef struct pke_cipher
{
  char v[2 * EFS_BN254 + 1];
  char c[6 * EFS_BN254 + 1];
  char t[32];
  octet V = {0, sizeof(v), v};
  octet C = {0, sizeof(c), c};
  octet T = {0, sizeof(t), t};
} pke_cipher_struct;

/* PKE specific functions */
/**
 * @brief Elliptic Curve Integrated Encryption Scheme (ECIES) setup
 * 
 * @param S private key
 * @param W public key
 * @param param encryption paramaters 
 * @param RNG  randon number generator
 * @return int 1 if setup fails, 0 otherwise
 */
int pke_setup(pke_S_struct *S, pke_W_struct *W, pke_param_struct *param, csprng RNG);

/**
 * @brief Elliptic Curve Integrated Encryption (ECIES) encryption
 * 
 * @param param encryption parameters
 * @param W public key
 * @param cipher cipher text
 * @param M plain text
 * @param RNG random number generator
 */
void pke_encryption(pke_param_struct param, octet W, pke_cipher_struct *cipher, octet M, csprng RNG);

/**
 * @brief 
 * 
 * @param param encryption parameters
 * @param U private key
 * @param cipher cipher text
 * @param M plain text
 * @return int 1 if fails, 0 otherwise
 */
int pke_decryption(pke_param_struct param, octet U, pke_cipher_struct cipher, octet *M);

#endif