#ifndef SPSEQ_H
#define SPSEQ_H

#include "miracl.h"

/* SPS-EQ specific types */
typedef struct spseq_sig_struct
{
  ECP Z;
  ECP Y;
  ECP2 Y_hat;
} spseq_sig_struct;

typedef struct bg_struct
{
  BIG p;
  ECP P;
  ECP2 P_hat;
} bg_struct;

typedef struct gsk_struct
{
  ECP R;
  ECP P;
} gsk_struct;

typedef struct spseq_key_struct
{
  BIG sk[2];
  ECP2 pk[2];
} spseq_key_struct;

/* SPEQ specific functions */

/**
 * @brief Set the ECP generator, ECP2 generator, and P the curve order
 * 
 * @param bg bilinear group description
 */
int spseq_bgGen(bg_struct *bg);

/**
 * @brief On input of bilinear-group description bg, the secret key sk is choosen and the public key pk is computed. 
 * 
 * @param bg bilinear group
 * @param sk private key
 * @param pk public key
 * @param RNG Random number generator
 */
int spseq_keyGen(bg_struct bg, BIG sk[], ECP2 pk[], csprng RNG);

/**
 * @brief On input ECP (U,Q) and secrect key sk signature sig is created
 * 
 * @param bg bilinear group
 * @param U message part 1
 * @param Q message part 2
 * @param sk secret key
 * @param sig output signature
 */
int spseq_sign(bg_struct bg, ECP U, ECP Q, BIG sk[], spseq_sig_struct *sig, csprng RNG);

/**
 * @brief On input ECP (U,Q) and the signature sig, the signature will be verified using public key pk
 * 
 * @param bg bilinear group
 * @param sig signature
 * @param pk public key
 * @param m0 message part 1
 * @param m1 message part 2
 */
int spseq_verify(bg_struct bg, spseq_sig_struct sig, ECP2 pk[], ECP U, ECP Q);

/**
 * @brief On input ECP (U,Q) and signature sig, a new randomized signature sig_random is created based on u
 * 
 * @param bg bilinear group
 * @param sig signature
 * @param sig_random output randomized signature
 * @param u randomization factor
 * @param U message part 1
 * @param Q message part 2
 */
int spseq_chgRep(bg_struct bg, spseq_sig_struct signature, spseq_sig_struct *signature_randomized, BIG u, ECP *m0, ECP *m1, csprng RNG);
#endif