#include "../include/spseq.h"

int spseq_bgGen(bg_struct *bg)
{
  // Get modulus p
  BIG_rcopy(bg->p, CURVE_Order);
  if(!ECP_generator(&bg->P)) // Create generator P
  {
    printf("\tSPSEQ ERROR, P not initialized correctly!\n"); 
    return EXIT_FAILURE;
  }

  if(!PAIR_G1member(&bg->P)) 
  {
    printf("\tSPSEQ ERROR, P not a member of G1\n");
    return EXIT_FAILURE;
  }

  // Create generator P_hat
  if(!ECP2_generator(&bg->P_hat)) 
  {
    printf("\tSPSEQ ERROR, P_hat not initialized correctly!\n"); 
    return EXIT_FAILURE;
  }

  if(!PAIR_G2member(&bg->P_hat)) 
  {
    printf("\tSPSEQ ERROR, P_hat not a member of G2\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int spseq_keyGen(bg_struct bg, BIG sk[], ECP2 pk[], csprng RNG)
{
  // Create secret key sk
  BIG_randtrunc(sk[0], bg.p, 2 * CURVE_SECURITY_BN254, &RNG);
  BIG_randtrunc(sk[1], bg.p, 2 * CURVE_SECURITY_BN254, &RNG);

  if(BIG_comp(sk[0], bg.p) == 1)
  {
    printf("\tSPSEQ ERROR sk[0] range is incorrect\n"); 
    return EXIT_FAILURE;
  } 
  if(BIG_comp(sk[1], bg.p) == 1)
  {
    printf("\tSPSEQ ERROR sk[1] range is incorrect\n"); 
    return EXIT_FAILURE;
  } 

  // Create public key pk
  ECP2_copy(&pk[0], &bg.P_hat);
  ECP2_copy(&pk[1], &bg.P_hat);
  ECP2_mul(&pk[0], sk[0]);
  ECP2_mul(&pk[1], sk[1]);

  if(!PAIR_G2member(&pk[0]))
  {
    printf("\tSPSEQ ERROR, pk[0] not a member of G2\n");
    return EXIT_FAILURE;
  } 

  if(!PAIR_G2member(&pk[1]))
  {
    printf("\tSPSEQ ERROR, pk[1] not a member of G2\n"); 
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int spseq_sign(bg_struct bg, ECP U, ECP Q, BIG sk[], spseq_sig_struct *sig, csprng RNG)
{
  BIG y, y_inv;
  BIG_randtrunc(y, bg.p, 2 * CURVE_SECURITY_BN254, &RNG);
  BIG_invmodp(y_inv, y, bg.p); // inv y
  if(BIG_comp(y_inv, bg.p) == 1)
  {
    printf("\tSPSEQ SIGN ERROR, y_inv range is incorrect\n"); 
    return EXIT_FAILURE;
  } 

  ECP_copy(&sig->Y, &bg.P);
  ECP_mul(&sig->Y, y_inv); // Y = inv y * P
  if(!PAIR_G1member(&sig->Y))
  {
    printf("\tSPSEQ SIGN ERROR, Y not a member of G1!\n"); 
    return EXIT_FAILURE;
  } 

  ECP2_copy(&sig->Y_hat, &bg.P_hat);
  ECP2_mul(&sig->Y_hat, y_inv); // Y_hat = inv * P_hat
  if(!PAIR_G2member(&sig->Y_hat))
  {
    printf("\tSPSEQ SIGN ERROR, Y_hat not a member of G2!\n"); 
    return EXIT_FAILURE;
  } 

  ECP ecp_temp; 
  ECP_copy(&sig->Z, &U);
  ECP_mul(&sig->Z, sk[0]); // x0U

  ECP_copy(&ecp_temp, &Q);
  ECP_mul(&ecp_temp, sk[1]); // x1Q 
  ECP_add(&sig->Z, &ecp_temp); // x1Q + x0U
  ECP_mul(&sig->Z, y); // y(x1Q + x0U)
  if(!PAIR_G1member(&sig->Z))
  {
    printf("\tSPSEQ SIGN ERROR, Z not a member of G1!\n"); 
    return EXIT_FAILURE;
  } 
  return EXIT_SUCCESS;
}

int spseq_verify(bg_struct bg, spseq_sig_struct sig, ECP2 pk[], ECP U, ECP Q)
{
  if(!PAIR_G1member(&sig.Y))
  {
    printf("\tSPSEQ VERIFY ERROR, Y not a member of G1!\n"); 
    return EXIT_FAILURE;
  } 

  if(!PAIR_G1member(&sig.Z))
  {
    printf("\tSPSEQ VERIFY ERROR, Z not a member of G1!\n"); 
    return EXIT_FAILURE;
  } 

  if(!PAIR_G2member(&sig.Y_hat))
  {
    printf("\tSPSEQ VERIFY ERROR, Y_hat not a member of G2!\n");
    return EXIT_FAILURE;
  } 
  
  FP12 v;
  ECP inv_Y;
  ECP_copy(&inv_Y, &sig.Y);
  ECP_neg(&inv_Y); // inv Y
  PAIR_double_ate(&v, &bg.P_hat, &inv_Y, &sig.Y_hat, &bg.P); // e(P_hat, Y_inv)e(Y_hat, P)
  PAIR_fexp(&v);
  if(!FP12_isunity(&v)){
    printf("\tSPSEQ ERROR, part 1 spseq signature does not verify!\n"); 
    return EXIT_FAILURE;
  } 

  FP12 w;
  ECP inv_Z;
  FP12_zero(&v);
  ECP_copy(&inv_Z, &sig.Z);
  ECP_neg(&inv_Z);
  PAIR_double_ate(&v, &pk[0], &U, &pk[1], &Q); // e(pk[0],U)e(pk[1],Q)
  PAIR_ate(&w, &sig.Y_hat, &inv_Z); // e(Y_hat, Z_inv)
  FP12_mul(&v, &w); // e(pk[0],U)e(pk[1],Q)e(Y_hat, Z_inv)
  PAIR_fexp(&v);
  if(!FP12_isunity(&v))
  {
    printf("\tSPSEQ ERROR, part 2 spseq signature does not verify!\n"); 
    return EXIT_FAILURE;
  } 
  return EXIT_SUCCESS;
}

int spseq_chgRep(bg_struct bg, spseq_sig_struct sig, spseq_sig_struct *sig_random, BIG u, ECP *U, ECP *Q, csprng RNG)
{
  if(!PAIR_G1member(&sig.Y))
  {
    printf("\tSPSEQ RANDOMIZE ERROR, Y not a member of G1!\n"); 
    return EXIT_FAILURE;
  }

  if(!PAIR_G1member(&sig.Z)) 
  {
    printf("\tSPSEQ RANDOMIZE ERROR, Z not a member of G1!\n"); 
    return EXIT_FAILURE;
  }

  if(!PAIR_G2member(&sig.Y_hat))
  {
    printf("\tSPSEQ RANDOMIZE ERROR, Y_hat not a member of G1!\n"); 
    return EXIT_FAILURE;
  }

  if(BIG_comp(u, bg.p) == 1) 
  {
    printf("\tSPSEQ RANDOMIZE ERROR, u range is incorrect\n"); 
    return EXIT_FAILURE;
  }

  BIG y, inv_y;
  BIG_randtrunc(y, bg.p, 2 * CURVE_SECURITY_BN254, &RNG);
  BIG_invmodp(inv_y, y, bg.p);

  ECP_copy(&sig_random->Z, &sig.Z);
  ECP_copy(&sig_random->Y, &sig.Y);
  ECP2_copy(&sig_random->Y_hat, &sig.Y_hat);

  ECP_mul(&sig_random->Z, u); // uZ
  ECP_mul(&sig_random->Z, y); // yuZ
  ECP_mul(&sig_random->Y, inv_y); // inv y * Y
  ECP2_mul(&sig_random->Y_hat, inv_y); // inv y * Y_hat

  ECP_mul(U, u);
  ECP_mul(Q, u);
  return EXIT_SUCCESS;
}