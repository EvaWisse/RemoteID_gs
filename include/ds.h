#ifndef DS_H
#define DS_H

#include "miracl.h"
#include "../include/miracl/bls_BN254.h"

typedef struct ds_sig_struct
{
	char sig[BFS_BN254 + 1];
	octet SIG = {0, sizeof(sig), sig};
} ds_sig_struct;

typedef struct ds_S_struct
{
	char s[BGS_BN254];
	octet S = {0, sizeof(s), s};	
} ds_S_struct;

typedef struct ds_W_struct
{
	char w[4 * BFS_BN254 + 1];
	octet W = {0, sizeof(w), w};	
} ds_W_struct;

/**
 * @brief Boneh–Lynn–Shacham (BLS) signature setup
 * 
 * @param key key
 * @param RNG randon number generator
 * @return int 1 if failed, 0 otherwise
 */
int ds_setup(ds_W_struct *W, ds_S_struct *S, csprng RNG);

/**
 * @brief BLS signing 
 * 
 * @param sig return signature
 * @param message the to be signed message
 * @param S secrect key
 * @return int 1 if failed, 0 otherwise
 */
int ds_sign(octet *sig, octet message, octet S);

/**
 * @brief BLS signature verification
 * 
 * @param sig signature of message
 * @param message signed message
 * @param W public key
 * @return int 1 if failed, 0 otherwise
 */
int ds_verify(octet sig, octet message, octet W);

#endif