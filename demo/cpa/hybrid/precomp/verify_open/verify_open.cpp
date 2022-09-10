#include "verify_open.h"

int main()
{
  if(fromFile()) return EXIT_FAILURE;
  if(verify()) return EXIT_FAILURE;
  if(open()) return EXIT_FAILURE;
  printf("Opened and verified\n");
  return EXIT_SUCCESS;
}
int verify()
{
  if(spseq_verify(shared.spseq_bg, drone_output.new_sig, shared.spseq_pk, drone_output.m1, drone_output.m2)) return EXIT_FAILURE;

  BIG t1;
  ECP T1, N;
  ECP_copy(&N, &shared.spseq_bg.P); // N = P
  ECP_mul(&N, drone_output.z1); // N = z1P
  ECP_copy(&T1, &drone_output.m2); // T1 = P'
  ECP_mul(&T1, drone_output.c); // T1 = cP'
  ECP_sub(&N, &T1); // N = z1P - cP 

  ECP2 T2, M1, M2;
  ECP2_copy(&M1, &drone_output.new_sig.Y_hat); 
  ECP2_mul(&M1, drone_output.z2); // z2*Y_hat
  ECP2_copy(&T2, &drone_output.C1); 
  ECP2_mul(&T2, drone_output.c); // cC1
  ECP2_sub(&M1, &T2); // M1 = z2Y_hat - cC1

  ECP2_copy(&T2, &drone_output.C2);
  ECP2_mul(&T2, drone_output.c); // cC2
  BIG_modadd(t1, drone_output.z1, drone_output.z2, shared.spseq_bg.p); // z1 + z2
  ECP2_copy(&M2, &shared.spseq_bg.P_hat);
  ECP2_mul(&M2, t1); // (z1 + z2)P_hat
  ECP2_sub(&M2, &T2); // (z1 + z2)P_hat - cC2 

  BIG c_new;
  char digest[32];
  hash256 sh256;
  HASH256_init(&sh256);
  hash_ECP(&sh256, N);
  hash_ECP2(&sh256, M1);
  hash_ECP2(&sh256, M2);
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
  return EXIT_SUCCESS;
}

int open()
{
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

  return EXIT_SUCCESS;
} 

int fromFile()
{
  FILE *fp = fopen("demo/cpa/hybrid/pre-comp_all/verify_open/group_info.txt", "r");
  if(!fp)
  {
    printf("\tERROR, could not open \"group_info.txt\"\n");
    return EXIT_FAILURE;
  }

  BIG_fromFile(fp,&shared.spseq_bg.p);
  ECP_fromFile(fp,&shared.spseq_bg.P);
  ECP2_fromFile(fp,&shared.spseq_bg.P_hat);
  ECP2_fromFile(fp,&shared.spseq_pk[0]);
  ECP2_fromFile(fp,&shared.spseq_pk[1]);
  OCT_fromFile(&ttp.reg.C.V.len, ttp.reg.C.V.val, fp);
  OCT_fromFile(&ttp.reg.C.C.len, ttp.reg.C.C.val, fp);
  OCT_fromFile(&ttp.reg.C.T.len, ttp.reg.C.T.val, fp);
  OCT_fromFile(&shared.pke_param.P1.len, shared.pke_param.P1.val, fp);
  OCT_fromFile(&shared.pke_param.P2.len, shared.pke_param.P2.val, fp);
  OCT_fromFile(&ttp.pke_sk.S.len, ttp.pke_sk.S.val, fp);
  fclose(fp);
  return EXIT_SUCCESS;
}