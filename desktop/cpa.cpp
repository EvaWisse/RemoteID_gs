#include "cpa.h"

int main()
{
  if(setup()) return EXIT_FAILURE;
  if(join()) return EXIT_FAILURE;
  if(sign()) return EXIT_FAILURE;
  if(verify()) return EXIT_FAILURE;
  if(open()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int setup()
{
  printf("Setup group...\n");
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len =100;
  CREATE_CSPRNG(&RNG,  &RAW);

  if(pke_setup(&ttp.pke_sk, &shared.pke_pk, &shared.pke_param, RNG)) return EXIT_FAILURE;
  if(spseq_bgGen(&shared.spseq_bg)) return EXIT_FAILURE;
  if(spseq_keyGen(shared.spseq_bg, ttp.spseq_sk, shared.spseq_pk, RNG)) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
} 

int join()
{
  printf("Join group...\n");
  if(join_part1()) return EXIT_FAILURE;
  if(join_ttp()) return EXIT_FAILURE;
  if(join_part2()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int join_part1()
{
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG, &RAW);

  // generate DS keys
  ds_setup(&shared_setup.ds_pk, &drone_setup.ds_sk, RNG);

  // get random q and r
  BIG r;
  BIG_randtrunc(r, shared.spseq_bg.p, 2 * CURVE_SECURITY_BN254, &RNG);
  BIG_randtrunc(drone_setup.q, shared.spseq_bg.p, 2 * CURVE_SECURITY_BN254, &RNG);

  // set U and Q
  ECP_copy(&shared_setup.Q, &shared.spseq_bg.P);
  ECP_mul(&shared_setup.Q, drone_setup.q);
  ECP_copy(&shared_setup.U, &shared_setup.Q);
  ECP_mul(&shared_setup.U, r);

  // set rP_hat
  ECP2 rP_hat;
  ECP2_copy(&rP_hat, &shared.spseq_bg.P_hat);
  ECP2_mul(&rP_hat, r);

  // encrypt rP_hat
  char rp_hat[4 * MODBYTES_B256_28 + 1];
  octet oct_rP_hat = {0, sizeof(rp_hat), rp_hat};
  ECP2_toOctet(&oct_rP_hat, &rP_hat, false);
  pke_encryption(shared.pke_param, shared.pke_pk.W, &shared_setup.pke_cipher, oct_rP_hat, RNG);

  // c = hash(pke_cipher.V|pke_cipher.C|pke_cipher.T)
  char digest[32];
  octet c ={0, sizeof(digest), digest};
  hash256 sh256;
  HASH256_init(&sh256);
  for (int i = 0; i < shared_setup.pke_cipher.V.len; i++) HASH256_process(&sh256, shared_setup.pke_cipher.V.val[i]);
  for (int i = 0; i < shared_setup.pke_cipher.C.len; i++) HASH256_process(&sh256, shared_setup.pke_cipher.C.val[i]);
  for (int i = 0; i < shared_setup.pke_cipher.T.len; i++) HASH256_process(&sh256, shared_setup.pke_cipher.T.val[i]);
  HASH256_hash(&sh256, c.val);
  c.len = 32;

  // sign c
  if(ds_sign(&shared_setup.ds_sig.SIG, c, drone_setup.ds_sk.S)) return EXIT_FAILURE;
  
  // prove knowledge of r
  ECP A; 
  BIG a; 
  BIG_fromBytesLen(a, c.val, c.len);
  ECP_copy(&A, &shared_setup.Q);
  ECP_mul(&A, a); // A = aQ
  nizk_prove(A, a, shared_setup.Q, &shared_setup.nizk_proof[0]);
  nizk_prove(shared_setup.U, r, shared_setup.Q, &shared_setup.nizk_proof[1]);

  return EXIT_SUCCESS;
}

int join_ttp()
{
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG,  &RAW);

  // sign (U, Q)  
  if(spseq_sign(shared.spseq_bg, shared_setup.U, shared_setup.Q, ttp.spseq_sk, &shared_setup.spseq_sig, RNG)) return EXIT_FAILURE;

  // c = hash(pke_cipher.V|pke_cipher.C|pke_cipher.T)
  char digest[32];
  octet c ={0, sizeof(digest), digest};
  hash256 sh256;
  HASH256_init(&sh256);
  for (int i = 0; i < shared_setup.pke_cipher.V.len; i++) HASH256_process(&sh256, shared_setup.pke_cipher.V.val[i]);
  for (int i = 0; i < shared_setup.pke_cipher.C.len; i++) HASH256_process(&sh256, shared_setup.pke_cipher.C.val[i]);
  for (int i = 0; i < shared_setup.pke_cipher.T.len; i++) HASH256_process(&sh256, shared_setup.pke_cipher.T.val[i]);
  HASH256_hash(&sh256, c.val);
  c.len = 32;

  // verify ds
  if(ds_verify(shared_setup.ds_sig.SIG, c, shared_setup.ds_pk.W)) return EXIT_FAILURE;
  
  ECP A; 
  BIG a; 
  BIG_fromBytesLen(a, c.val, c.len);
  ECP_copy(&A, &shared_setup.Q);
  ECP_mul(&A, a); // A = aQ
  if(nizk_verify(A, shared_setup.Q, shared_setup.nizk_proof[0])) return EXIT_FAILURE;
  if(nizk_verify(shared_setup.U, shared_setup.Q, shared_setup.nizk_proof[1])) return EXIT_FAILURE;

  // set reg
  OCT_copy(&ttp.reg.C.C, &shared_setup.pke_cipher.C);
  OCT_copy(&ttp.reg.C.V, &shared_setup.pke_cipher.V);
  OCT_copy(&ttp.reg.C.T, &shared_setup.pke_cipher.T);
  OCT_copy(&ttp.reg.sig.SIG, &shared_setup.ds_sig.SIG);

  return EXIT_SUCCESS;
}

int join_part2()
{
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG, &RAW);

  ECP_copy(&drone.m1, &shared_setup.U);
  ECP_copy(&drone.m2, &shared_setup.Q);
  
  BIG inv_q;
  BIG_invmodp(inv_q, drone_setup.q, shared.spseq_bg.p);

  if(spseq_chgRep(shared.spseq_bg, shared_setup.spseq_sig, &drone.sig, inv_q, &drone.m1, &drone.m2, RNG)) return EXIT_FAILURE;
  if(spseq_verify(shared.spseq_bg, drone.sig, shared.spseq_pk, drone.m1, drone.m2)) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int sign()
{
  printf("Sign message...\n");
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG,  &RAW);

  BIG v, rho;
  BIG_randtrunc(rho, shared.spseq_bg.p, 2 * CURVE_SECURITY_BN254, &RNG);// select r in {1, ..., p-1}
  BIG_randtrunc(v, shared.spseq_bg.p, 2 * CURVE_SECURITY_BN254, &RNG);// select v in {1, ..., p-1}

  ECP_copy(&drone_output.m1, &drone.m1);
  ECP_copy(&drone_output.m2, &drone.m2);

  if(spseq_chgRep(shared.spseq_bg, drone.sig, &drone_output.new_sig , rho, &drone_output.m1, &drone_output.m2, RNG)) return EXIT_FAILURE;
  // if(spseq_verify(shared.spseq_bg, *random, spseq_pk, drone.m1, drone.m2)) return EXIT_FAILURE;

  // Calculate N
  ECP N;
  ECP_copy(&N, &shared.spseq_bg.P);
  ECP_mul(&N, v); // vP

  // Create hash
  hash256 sh256;
  char c_ch[32];
  HASH256_init(&sh256);
  hash_ECP(&sh256, drone_output.m1);
  hash_ECP(&sh256, drone_output.m2);
  hash_ECP(&sh256, N);
  hash_ECP(&sh256, drone_output.new_sig.Z);
  hash_ECP(&sh256, drone_output.new_sig.Y);
  hash_ECP2(&sh256, drone_output.new_sig.Y_hat);
  for (int i = 0; i < 12; i++) HASH256_process(&sh256, drone_output.m.uav_loc[i]);
  for (int i = 0; i < 12; i++) HASH256_process(&sh256, drone_output.m.gs_loc[i]);
  for (int i = 0; i < 6; i++) HASH256_process(&sh256, drone_output.m.uav_v[i]);
  for (int i = 0; i < 4; i++) HASH256_process(&sh256, drone_output.m.ts[i]);
  HASH256_process(&sh256, drone_output.m.es);
  HASH256_hash(&sh256, c_ch);

  // digest to big num
  BIG c;
  BIG_fromBytesLen(drone_output.c, c_ch, 32);

  // Calculate z1 
  BIG temp;
  BIG_zero(temp);
  BIG_modmul(temp, drone_output.c, rho, shared.spseq_bg.p); // c * rho
  BIG_modadd(drone_output.z1, temp, v, shared.spseq_bg.p); // v + c * rho

  return EXIT_SUCCESS;
}

int verify()
{
  printf("Verify message...\n");

  if(spseq_verify(shared.spseq_bg, drone_output.new_sig, shared.spseq_pk, drone_output.m1, drone_output.m2)) return EXIT_FAILURE;

  BIG t1;
  ECP N, T1;
  ECP_copy(&N, &shared.spseq_bg.P); // N = P
  ECP_mul(&N, drone_output.z1); // N = z1P
  ECP_copy(&T1, &drone_output.m2); // T1 = P'
  ECP_mul(&T1, drone_output.c); // T1 = cP'
  ECP_sub(&N, &T1); // N = z1P - cP 


  BIG c_new;
  char digest[32];
  hash256 sh256;
  HASH256_init(&sh256);
  hash_ECP(&sh256, drone_output.m1);
  hash_ECP(&sh256, drone_output.m2);
  hash_ECP(&sh256, N);
  hash_ECP(&sh256, drone_output.new_sig.Z);
  hash_ECP(&sh256, drone_output.new_sig.Y);
  hash_ECP2(&sh256, drone_output.new_sig.Y_hat);
  for (int i = 0; i < 12; i++) HASH256_process(&sh256, drone_output.m.uav_loc[i]);
  for (int i = 0; i < 12; i++) HASH256_process(&sh256, drone_output.m.gs_loc[i]);
  for (int i = 0; i < 6; i++) HASH256_process(&sh256, drone_output.m.uav_v[i]);
  for (int i = 0; i < 4; i++) HASH256_process(&sh256, drone_output.m.ts[i]);
  HASH256_process(&sh256, drone_output.m.es);
  HASH256_hash(&sh256, digest);
  BIG_fromBytesLen(c_new, digest, 32);

  if(BIG_comp(drone_output.c,c_new))
  {
    printf("\tVERIFY ERROR, hash does not verify.\n");
    return EXIT_FAILURE;
  }
  printf("Verification successful!\n");
  return EXIT_SUCCESS;
}

int open()
{
  printf("Open message...\n");
  // decrypt
  ECP2 rP_hat;
  char rp_hat[4 * MODBYTES_B256_28 + 1];
  octet oct_rP_hat = {0, sizeof(rp_hat), rp_hat};
  if(pke_decryption(shared.pke_param, ttp.pke_sk.S, shared_setup.pke_cipher, &oct_rP_hat)) return EXIT_FAILURE;
  ECP2_fromOctet(&rP_hat, &oct_rP_hat);

  FP12 v;
  ECP inv_m2;
  ECP_copy(&inv_m2, &drone_output.m2);
  ECP_neg(&inv_m2);
  PAIR_double_ate(&v, &rP_hat, &inv_m2, &shared.spseq_bg.P_hat, &drone_output.m1); // (rP,P_hat)e(rP,P_hat)
  PAIR_fexp(&v);
  if(!FP12_isunity(&v)){
    printf("\tOpen ERROR, does not open!\n"); 
    return EXIT_FAILURE;
  } 
  printf("Opening successful!\n");
  return EXIT_SUCCESS;
} 