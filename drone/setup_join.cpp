#include "setup_join.h"

#define PRE_COMP // if selected pre-comp version is used
#define FLIGHT_TIME 5 // number of pre-comp.

int main()
{
  if(setup()) return EXIT_FAILURE;
  if(join()) return EXIT_FAILURE;
  if(to_file()) return EXIT_FAILURE;
  if(to_header()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int setup()
{
  printf("\tCreating group...\n");
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
  printf("\tJoining group...\n");
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

int to_file()
{
  printf("\tSaving credentials and group info...\n");
  FILE *fp = fopen("drone/group_info.txt", "wb");
  if(!fp)
  {
    printf("Could not create group_info.txt\n");
    return EXIT_FAILURE;
  }
  ECP2_toFile(shared.spseq_pk[0], fp);
  ECP2_toFile(shared.spseq_pk[1], fp);

  OCT_toFile(shared.pke_param.P1, fp);
  OCT_toFile(shared.pke_param.P2, fp);

  OCT_toFile(ttp.pke_sk.S, fp);

  OCT_toFile(shared_setup.pke_cipher.C, fp);
  OCT_toFile(shared_setup.pke_cipher.T, fp);
  OCT_toFile(shared_setup.pke_cipher.V, fp);
  fclose(fp);

  return EXIT_SUCCESS;
}

int to_header()
{
  printf("\tCreating drone constants as a header file...\n");
  FILE *fp = fopen("drone/drone_const.h", "w");
  if(!fp)
  {
    printf("Could not create drone_const.h\n");
    return EXIT_FAILURE;
  }
  
  fprintf(fp, "#include \"../include/miracl.h\"\n#ifndef CONST_UAS_H\n#define CONST_UAS_H\n");
  fprintf(fp, "BIG m1_h[3] ={{");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", drone.m1.x.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", drone.m1.y.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", drone.m1.z.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}};\n");

  fprintf(fp, "BIG m2_h[3] ={{");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", drone.m2.x.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", drone.m2.y.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", drone.m2.z.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}};\n");

  fprintf(fp, "BIG Y_h[3] ={{");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y.x.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y.y.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y.z.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}};\n");

  fprintf(fp, "BIG Z_h[3] ={{");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Z.x.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Z.y.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Z.z.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}};\n");

  fprintf(fp, "BIG Y_hat_ha[3] ={{");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y_hat.x.a.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y_hat.y.a.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y_hat.z.a.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}};\n");

  fprintf(fp, "BIG Y_hat_hb[3] ={{");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y_hat.x.b.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y_hat.y.b.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}, {");
  for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", shared_setup.spseq_sig.Y_hat.z.b.g[i]);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "}};\n");

  fprintf(fp, "const sign32 xesa[3]= ");
  fprintf(fp, "{%#04x, %#04x, %#04x};\n", shared_setup.spseq_sig.Y_hat.x.a.XES, shared_setup.spseq_sig.Y_hat.y.a.XES, shared_setup.spseq_sig.Y_hat.z.a.XES );
  fprintf(fp, "const sign32 xesb[3]= ");
  fprintf(fp, "{%#04x, %#04x, %#04x};\n", shared_setup.spseq_sig.Y_hat.x.b.XES, shared_setup.spseq_sig.Y_hat.y.b.XES, shared_setup.spseq_sig.Y_hat.z.b.XES );

  fprintf(fp, "const sign32 Z_xes[3]= ");
  fprintf(fp, "{%#04x, %#04x, %#04x};\n", shared_setup.spseq_sig.Z.x.XES, shared_setup.spseq_sig.Z.y.XES, shared_setup.spseq_sig.Z.z.XES);
  fprintf(fp, "const sign32 Y_xes[3]= ");
  fprintf(fp, "{%#04x, %#04x, %#04x};\n", shared_setup.spseq_sig.Y.x.XES, shared_setup.spseq_sig.Y.y.XES, shared_setup.spseq_sig.Y.z.XES);

  fprintf(fp, "const sign32 m1_xes[3]= ");
  fprintf(fp, "{%#04x, %#04x, %#04x};\n", drone.m1.x.XES, drone.m1.y.XES, drone.m1.z.XES);
  fprintf(fp, "const sign32 m2_xes[3]= ");
  fprintf(fp, "{%#04x, %#04x, %#04x};\n", drone.m2.x.XES, drone.m2.y.XES, drone.m2.z.XES);
  fprintf(fp, "#endif\n");

  #ifdef PRE_COMP

  #endif

  fclose(fp);

  return EXIT_SUCCESS;
}