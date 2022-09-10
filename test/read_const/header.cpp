#include "../../include/miracl.h"
#include "../../include/utils.h"

#define rounds 1
#define WRITE

#ifdef WRITE
int main()
{
  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG, &RAW);

  BIG big[rounds];
  ECP ecp[rounds], P;
  ECP_generator(&P);
  FILE *FP=fopen("back.txt", "w");
  
  FILE *fp = fopen("header.h", "w");
  fprintf(fp, "#include \"include/miracl.h\"\n#ifndef CONST_ECP_UAS_H\n#define CONST_ECP_UAS_H\n");
  fprintf(fp, "const BIG test={");
  for(int i=0;i<rounds; i++)
  {
    BIG_random(big[i], &RNG);
    ECP_copy(&ecp[i], &P);
    ECP_mul(&ecp[i], big[i]);
    ECP_precomp(i,ecp[i], fp);
    // ECP_toFile(ecp[i], FP);
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");
  fprintf(fp, "#endif\n");
  fclose(fp);
  fclose(FP);
}
#else
#include "header.h"
int main()
{
  BIG big[rounds];
  ECP ecp[rounds], P, check_ecp;
  BIG x, y;  
  

  BIG_rcopy(x, ECP0_x);
  BIG_rcopy(y, ECP0_y);
  ECP_set(&ecp[0], x, y);
  ECP_output(&ecp[0]);
  printf("\n");

  FILE *fp = fopen("back.txt", "r");
  ECP_fromFile(fp, check_ecp);
  if(ECP_equals(&check_ecp, ecp[0]))

  //   BIG_rcopy(x, ECP1_x);
  // BIG_rcopy(y, ECP1_y);
  // ECP_set(&ecp[1], x, y);
  // ECP_output(&ecp[1]);
  // printf("\n");

  //   BIG_rcopy(x, ECP2_x);
  // BIG_rcopy(y, ECP2_y);
  // ECP_set(&ecp[2], x, y);
  // ECP_output(&ecp[2]);
  // printf("\n");

  //   BIG_rcopy(x, ECP2_x);
  // BIG_rcopy(y, ECP2_y);
  // ECP_set(&ecp[2], x, y);
  // ECP_output(&ecp[2]);
  // printf("\n");


  
}
#endif