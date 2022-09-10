#include "setup.h"

int main()
{
  if(setup()) return EXIT_FAILURE;
  if(join()) return EXIT_FAILURE;
  if(toHeader()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int setup()
{
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

int toHeader()
{
  FILE *fp = fopen("demo/cca2/hybrid/original/sign/drone_const.h", "w");

  if(!fp)
  {
    printf("\tERROR, could not create \"drone_const.h\"\n");
    return EXIT_FAILURE;
  }
  fprintf(fp, "#include\"exfunc.h\"\n#ifndef CONST_ECP_UAS_H\n#define CONST_ECP_UAS_H\n");
  fprintf(fp, "char ch_m1[] =\"");
  ECP_toheader(drone.m1, fp);
  fprintf(fp, "\";\nchar ch_m2[] =\"");
  ECP_toheader(drone.m2, fp);
  fprintf(fp, "\";\nchar ch_Y[] =\"");
  ECP_toheader(shared_setup.spseq_sig.Y, fp);
  fprintf(fp, "\";\nchar ch_Z[] =\"");
  ECP_toheader(shared_setup.spseq_sig.Z, fp);
  fprintf(fp, "\";\nchar ch_Y_hat[] =\"");
  ECP2_toheader(shared_setup.spseq_sig.Y_hat, fp);
  fprintf(fp, "\";\n#endif\n");
  fclose(fp);
  return EXIT_SUCCESS;
}