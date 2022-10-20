#include "../include/miracl.h"
#include "../include/utils.h"
#define ROUNDS 100

BIG big[ROUNDS], p;
ECP ecp[ROUNDS], ecp_file[ROUNDS], g1;
ECP2 ecp2[ROUNDS], ecp2_file[ROUNDS], g2;

int main()
{
  BIG_rcopy(p, CURVE_Order);
  ECP_generator(&g1);
  ECP2_generator(&g2);

  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len =100;
  CREATE_CSPRNG(&RNG,  &RAW);

  char ch[4 * MODBYTES_B256_28 + 4];
  char ch_file[4 * MODBYTES_B256_28 + 4];
  octet oct = {0, sizeof(ch), ch};
  octet oct_file = {0, sizeof(ch_file), ch_file};

  FILE *fp = fopen("test/test.txt", "wb");
  for(int i=0; i<ROUNDS; i++)
  {
    BIG_randomnum(big[i], p, &RNG);
    ECP_copy(&ecp[i], &g1);
    ECP2_copy(&ecp2[i], &g2);
    ECP_mul(&ecp[i], big[i]);
    ECP2_mul(&ecp2[i], big[i]);

    BIG_toFile(big[i], fp);
    ECP_toFile(ecp[i], fp);
    ECP2_toFile(ecp2[i], fp);
  }

  for(int i=0; i<ROUNDS; i++)
  {
    OCT_clear(&oct);
    OCT_rand(&oct, &RNG, 71);
    OCT_toFile(&oct, fp);
    OCT_output(&oct);
  }
  fclose(fp);

  printf("\n\nreading files:\n");
  fp = fopen("test/test.txt", "rb");
  for(int i=0; i<ROUNDS; i++)
  {
    BIG_fromFile(fp, &big[i]);
    ECP_fromFile(fp, &ecp_file[i]);
    ECP2_fromFile(fp, &ecp2_file[i]);

    ECP_copy(&ecp[i], &g1);
    ECP2_copy(&ecp2[i], &g2);

    ECP_mul(&ecp[i], big[i]);
    ECP2_mul(&ecp2[i], big[i]);

    if(!ECP_equals(&ecp[i], &ecp_file[i])) printf("%d, FAILED ECP\n", i);
    if(!ECP2_equals(&ecp2[i], &ecp2_file[i])) printf("%d, FAILED ecp2\n", i);
  }
  for(int i=0; i<ROUNDS; i++)
  {
    OCT_fromFile(&oct_file, fp);
    OCT_output(&oct_file);
    OCT_clear(&oct_file);
  }

  fclose(fp);
}