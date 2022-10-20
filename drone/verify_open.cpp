#include "verify_open.h"
#include "setting.h"

int main()
{
  if(read_groupinfo()) return EXIT_FAILURE;
  if(verify()) return EXIT_FAILURE;
  if(open()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int verify()
{
  printf("\tVerifying broadcast...\n");
  read_bc();

  BIG t1;
  char m[32];
  ECP T1, N;
  ECP_copy(&N, &shared.spseq_bg.P); // N = P
  ECP_mul(&N, drone_output.z1); // N = z1P
  ECP_copy(&T1, &drone_output.m2); // T1 = P'
  ECP_mul(&T1, drone_output.c); // T1 = cP'
  ECP_sub(&N, &T1); // N = z1P - cP 

  #ifdef CCA2_SIGNATURE
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
  #endif

  BIG c_new;
  char digest[32];
  hash256 sh256;
  HASH256_init(&sh256);  
  hash_ECP(&sh256, drone_output.m1);
  hash_ECP(&sh256, drone_output.m2);
  hash_ECP(&sh256, drone_output.new_sig.Z);
  hash_ECP(&sh256, drone_output.new_sig.Y);
  hash_ECP2(&sh256, drone_output.new_sig.Y_hat);
  hash_ECP(&sh256, N);
  #ifdef CCA2_SIGNATURE
    hash_ECP2(&sh256, M1);
    hash_ECP2(&sh256, M2);
  #endif
  memset(m, 0, m_size);
  for (int i = 0; i < m_size; i++) HASH256_process(&sh256, m[i]);
  HASH256_hash(&sh256, digest);
  BIG_fromBytesLen(c_new, digest, 32);

  if(BIG_comp(drone_output.c,c_new))
  {
    printf("\tVERIFY ERROR, hash does not verify.\n");
    return EXIT_FAILURE;
  }
  printf("\tBroadcast verified!\n");
  return EXIT_SUCCESS;
}

int open()
{
  printf("\tOpening broadcast...\n");
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
  printf("\tBroadcast opened!\n");
  return EXIT_SUCCESS;
} 

int read_groupinfo()
{
  printf("\tReading group information...\n");
  FILE *fp = fopen("drone/group_info.txt", "rb");
  if(!fp)
  {
    printf("Could not open group_info.txt\n");
    return EXIT_FAILURE;
  }
  ECP2_fromFile(fp, &shared.spseq_pk[0]);
  ECP2_fromFile(fp, &shared.spseq_pk[1]);
  OCT_fromFile(&shared.pke_param.P1, fp);
  OCT_fromFile(&shared.pke_param.P2, fp);
  OCT_fromFile(&ttp.pke_sk.S, fp);
  OCT_fromFile(&shared_setup.pke_cipher.C, fp);
  OCT_fromFile(&shared_setup.pke_cipher.T, fp);
  OCT_fromFile(&shared_setup.pke_cipher.V, fp);

  fclose(fp);

  ECP_generator(&shared.spseq_bg.P);
  ECP2_generator(&shared.spseq_bg.P_hat);
  BIG_rcopy(shared.spseq_bg.p, CURVE_Order);

  return EXIT_SUCCESS;
}

int read_bc()
{
  printf("\tReading and parsing broadcast...\n");
  memset(bc, 0,  4 * ecp_size + 3 * ecp2_size + m_size + 3* big_size);
  FILE *fp = fopen("drone/broadcast.txt", "rb");
  if(!fp)
  {
    printf("Could not open broadcast.txt\n");
    return EXIT_FAILURE;
  }
  #ifdef CCA2_SIGNATURE    
    fread(bc, sizeof(char), 4 * ecp_size + 3 * ecp2_size + m_size + 3 * big_size, fp);
  #else
    fread(bc, sizeof(char), 4 * ecp_size + ecp2_size + m_size + 2 * big_size, fp);
  #endif
  fclose(fp);

  ECP_fromChar(&drone_output.m1, bc);
  ECP_fromChar(&drone_output.m2, bc + ecp_size);
  ECP_fromChar(&drone_output.new_sig.Z, bc + 2 * ecp_size);
  ECP_fromChar(&drone_output.new_sig.Y, bc + 3 * ecp_size);
  ECP2_fromChar(&drone_output.new_sig.Y_hat, bc + 4 * ecp_size);

  #ifdef CCA2_SIGNATURE
    ECP2_fromChar(&drone_output.C1, bc + 4 * ecp_size + ecp2_size);
    ECP2_fromChar(&drone_output.C2, bc + 4 * ecp_size + 2 * ecp2_size);
    BIG_fromBytesLen(drone_output.c, bc + 4 * ecp_size + 3 * ecp2_size + m_size, big_size);
    BIG_fromBytesLen(drone_output.z1, bc + 4 * ecp_size + 3 * ecp2_size + m_size + big_size, big_size);
    BIG_fromBytesLen(drone_output.z2, bc + 4 * ecp_size + 3 * ecp2_size + m_size + 2 * big_size, big_size);
  #else
    BIG_fromBytesLen(drone_output.c, bc + 4 * ecp_size + ecp2_size + m_size, big_size);
    BIG_fromBytesLen(drone_output.z1, bc + 4 * ecp_size + ecp2_size + m_size + big_size, big_size);
  #endif 

  return EXIT_SUCCESS;
}