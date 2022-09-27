#include "setup.h"
#define FLIGHT_TIME 10 // in seconds
int main()
{
  printf("Start setup phase\n");
  if(setup()) return EXIT_FAILURE;
  if(join()) return EXIT_FAILURE;
  if(toFile())  return EXIT_FAILURE;
  if(toHeader()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int setup()
{
  printf("Setup phase....");
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len =100;
  CREATE_CSPRNG(&RNG,  &RAW);

  if(pke_setup(&ttp.pke_sk, &shared.pke_pk, &shared.pke_param, RNG)) return EXIT_FAILURE;
  if(spseq_bgGen(&shared.spseq_bg)) return EXIT_FAILURE;
  if(spseq_keyGen(shared.spseq_bg, ttp.spseq_sk, shared.spseq_pk, RNG)) return EXIT_FAILURE;
  
  printf("done\n");
  return EXIT_SUCCESS;
} 

int join()
{
  printf("Join phase...");
  if(join_part1()) return EXIT_FAILURE;
  if(join_ttp()) return EXIT_FAILURE;
  if(join_part2()) return EXIT_FAILURE;
  printf("done\n");

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

int toFile()
{
  printf("To file....");
  FILE *fp = fopen("demo/cpa/hybrid/precomp/verify_open/group_info.txt", "w");
  if(!fp)
  {
    printf("\tERROR, could not create \"group_info.txt\"\n");
    return EXIT_FAILURE;
  }
  BIG_toFile(shared.spseq_bg.p, fp);
  ECP_toFile(shared.spseq_bg.P, fp);
  ECP2_toFile(shared.spseq_bg.P_hat, fp);
  ECP2_toFile(shared.spseq_pk[0], fp);
  ECP2_toFile(shared.spseq_pk[1], fp);
  OCT_toFile(ttp.reg.C.V, fp);
  OCT_toFile(ttp.reg.C.C, fp);
  OCT_toFile(ttp.reg.C.T, fp);
  OCT_toFile(shared.pke_param.P1, fp);
  OCT_toFile(shared.pke_param.P2, fp);
  OCT_toFile(ttp.pke_sk.S, fp);
  fclose(fp);
  printf("done\n");
  return EXIT_SUCCESS;
}

int toHeader()
{
  printf("To header....");

  ECP Z_pre[FLIGHT_TIME][FLIGHT_TIME], Y_pre[FLIGHT_TIME], N_pre[FLIGHT_TIME], m1_pre[FLIGHT_TIME], m2_pre[FLIGHT_TIME];
  ECP2 Y_hat_pre[FLIGHT_TIME];
  BIG y[FLIGHT_TIME], rho[FLIGHT_TIME], v[FLIGHT_TIME], inv[FLIGHT_TIME], big;
  csprng RNG;
  char raw[100];
  RAND_seed(&RNG, 100, raw);

  for(int i =0; i<FLIGHT_TIME; i++)
  {
    BIG_randomnum(y[i], shared.spseq_bg.p, &RNG);
    BIG_randomnum(rho[i], shared.spseq_bg.p, &RNG);
    BIG_randomnum(v[i], shared.spseq_bg.p, &RNG);
    BIG_invmodp(inv[i], y[i], shared.spseq_bg.p);

    ECP_copy(&Y_pre[i], &drone.sig.Y);
    ECP_mul(&Y_pre[i], inv[i]);

    ECP_copy(&m1_pre[i], &drone.m1);
    ECP_mul(&m1_pre[i], rho[i]);

    ECP_copy(&m2_pre[i], &drone.m2);
    ECP_mul(&m2_pre[i], rho[i]);

    ECP2_copy(&Y_hat_pre[i], &drone.sig.Y_hat);
    ECP2_mul(&Y_hat_pre[i], inv[i]);

    ECP_copy(&N_pre[i], &shared.spseq_bg.P);
    ECP_mul(&N_pre[i], v[i]);

    for(int j=0; j<FLIGHT_TIME; j++)
    {
      BIG_modmul(big, rho[i], y[j], shared.spseq_bg.p); /// i = rho, j=y
      ECP_copy(&Z_pre[i][j], &drone.sig.Z);
      ECP_mul(&Z_pre[i][j], big);
    }
  }
  
  FILE *fp = fopen("demo/cpa/hybrid/precomp/sign/drone_const.h", "w");

  if(!fp)
  {
    printf("\tERROR, could not create \"drone_const.h\"\n");
    return EXIT_FAILURE;
  }
  fprintf(fp, "#include \"exfunc.h\"\n#ifndef CONST_ECP_UAS_H\n#define CONST_ECP_UAS_H\n");

  // Y 
  fprintf(fp, "BIG Y_x[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_pre[j].x.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_y[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_pre[j].y.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_z[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{"); 
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_pre[j].z.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  
  fprintf(fp, "};\nconst sign32 Y_xes[%d][3]= {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", Y_pre[j].x.XES);
    fprintf(fp, "%#04x, ", Y_pre[j].y.XES);
    fprintf(fp, "%#04x, ", Y_pre[j].z.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");

  // Z
  fprintf(fp, "BIG Z_x[%d][%d] = {", FLIGHT_TIME, FLIGHT_TIME);
  for(int i=0; i<FLIGHT_TIME; i++)
  {
    fprintf(fp, "{");
    for(int j=0; j<FLIGHT_TIME; j++)
    {
      fprintf(fp, "{");
      for(int k=0;k<NLEN_B256_28;k++) fprintf(fp, "%#08x, ", Z_pre[i][j].x.g[k]);
      fseek(fp, -2, SEEK_CUR);
      fprintf(fp, "}, ");
    }
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Z_y[%d][%d] = {", FLIGHT_TIME, FLIGHT_TIME);
  for(int i=0; i<FLIGHT_TIME; i++)
  {
    fprintf(fp, "{");
    for(int j=0; j<FLIGHT_TIME; j++)
    {
      fprintf(fp, "{");
      for(int k=0;k<NLEN_B256_28;k++) fprintf(fp, "%#08x, ", Z_pre[i][j].y.g[k]);
      fseek(fp, -2, SEEK_CUR);
      fprintf(fp, "}, ");
    }
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Z_z[%d][%d] = {", FLIGHT_TIME, FLIGHT_TIME);
  for(int i=0; i<FLIGHT_TIME; i++)
  {
    fprintf(fp, "{");
    for(int j=0; j<FLIGHT_TIME; j++)
    {
      fprintf(fp, "{");
      for(int k=0;k<NLEN_B256_28;k++) fprintf(fp, "%#08x, ", Z_pre[i][j].z.g[k]);
      fseek(fp, -2, SEEK_CUR);
      fprintf(fp, "}, ");
    }
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  
  fprintf(fp, "};\nconst sign32 Z_xes[%d][%d][3]= {", FLIGHT_TIME, FLIGHT_TIME);
  for(int i=0; i<FLIGHT_TIME; i++)
  {
    fprintf(fp, "{");
    for(int j=0; j<FLIGHT_TIME; j++)
    {
      fprintf(fp, "{%#04x, ", Z_pre[i][j].x.XES);
      fprintf(fp, "%#04x, ", Z_pre[i][j].y.XES);
      fprintf(fp, "%#04x}, ", Z_pre[i][j].z.XES);
    }
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");

  // Y_HAT
  fprintf(fp, "BIG Y_hat_xa[%d]={", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_hat_pre[j].x.a.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_hat_xb[%d]={", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_hat_pre[j].x.b.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_hat_ya[%d]={", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_hat_pre[j].y.a.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_hat_yb[%d]={", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_hat_pre[j].y.b.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_hat_za[%d]={", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_hat_pre[j].z.a.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG Y_hat_zb[%d]={", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", Y_hat_pre[j].z.b.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);


  fprintf(fp, "};\nconst sign32 xesa[%d][3]= {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", Y_hat_pre[j].x.a.XES);
    fprintf(fp, "%#04x, ", Y_hat_pre[j].y.a.XES);
    fprintf(fp, "%#04x, ", Y_hat_pre[j].z.a.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nconst sign32 xesb[%d][3]= {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", Y_hat_pre[j].x.b.XES);
    fprintf(fp, "%#04x, ", Y_hat_pre[j].y.b.XES);
    fprintf(fp, "%#04x, ", Y_hat_pre[j].z.b.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");

  // N
  fprintf(fp, "BIG N_x[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", N_pre[j].x.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG N_y[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", N_pre[j].y.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG N_z[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", N_pre[j].z.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  
  fprintf(fp, "};\nconst sign32 N_xes[%d][3]= {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", N_pre[j].x.XES);
    fprintf(fp, "%#04x, ", N_pre[j].y.XES);
    fprintf(fp, "%#04x, ", N_pre[j].z.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");

  // m1
  fprintf(fp, "BIG m1_x[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", m1_pre[j].x.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG m1_y[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", m1_pre[j].y.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG m1_z[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", m1_pre[j].z.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  
  fprintf(fp, "};\nconst sign32 m1_xes[%d][3]= {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", m1_pre[j].x.XES);
    fprintf(fp, "%#04x, ", m1_pre[j].y.XES);
    fprintf(fp, "%#04x, ", m1_pre[j].z.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");

  
  // m2
  fprintf(fp, "BIG m2_x[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", m2_pre[j].x.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG m2_y[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", m2_pre[j].y.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG m2_z[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", m2_pre[j].z.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  
  fprintf(fp, "};\nconst sign32 m2_xes[%d][3]= {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", m2_pre[j].x.XES);
    fprintf(fp, "%#04x, ", m2_pre[j].y.XES);
    fprintf(fp, "%#04x, ", m2_pre[j].z.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG rho[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", rho[j][i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG v[%d] = {", FLIGHT_TIME);
  for(int j=0; j<FLIGHT_TIME; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", v[j][i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\n#endif");
  fclose(fp);
  printf("done\n");
  return EXIT_SUCCESS;
}