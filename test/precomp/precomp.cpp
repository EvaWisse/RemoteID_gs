

#include "../../include/miracl.h"

void ECP_toHeader(int index, ECP ecp, FILE *fp);
void ECP2_toHeader(int index, ECP2 ecp2, FILE *fp);
void BIG_toCon(BIG x, FILE *fp);

void ECP_precomp(int index, ECP ecp, FILE *fp);
void ECP2_precomp(int index, ECP2 ecp2, FILE *fp);

#define rounds 10
#define WRITE

#ifdef WRITE
int main()
{

  ECP ecp1, ecp2, ecp3, ecp4;
  ECP_generator(&ecp1);
  ECP_generator(&ecp2);
  ECP_generator(&ecp3);
  ECP_generator(&ecp4);

  ECP_pinmul(&ecp2, 2, 8);
  ECP_pinmul(&ecp3, 3, 8);
  ECP_pinmul(&ecp4, 3, 8);

  ECP_output(&ecp1);
   printf("\n");
  ECP_output(&ecp2); 
  printf("\n");
  ECP_output(&ecp3); 
  printf("\n");
  ECP_output(&ecp4); 
  printf("\n");
  
  // write values to file
  FILE *fp =  fopen("test/precomp/precomp.h", "w");
  if(!fp)
  {
    printf("\tERROR, could not create \"precomp.h\"\n");
    return EXIT_FAILURE;
  }

BIG x;
  fprintf(fp, "#include\"../../include/miracl.h\"\n#ifndef PRECOMP_UAS_H\n#define PRECOMP_UAS_H\n");
  fprintf(fp, "const BIG m1[4]={");
   fprintf(fp, "{0X");
  FP_reduce(&(ecp1.x));
  FP_redc(x, &(ecp1.x));
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");
  
  fprintf(fp, "{0X");
  FP_reduce(&(ecp1.y));
  FP_redc(x, &(ecp1.y));
  BIG_toCon(x,  fp);
  fprintf(fp,"}} ,");

    fprintf(fp, "{{0x");
  FP_reduce(&(ecp2.x));
  FP_redc(x, &(ecp2.x));
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");
  
  fprintf(fp, "{0X");
  FP_reduce(&(ecp2.y));
  FP_redc(x, &(ecp2.y));
  BIG_toCon(x,  fp);
  fprintf(fp,"}} ");

   fprintf(fp, "{0X");
  FP_reduce(&(ecp3.x));
  FP_redc(x, &(ecp3.x));
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");
  
  fprintf(fp, "{0X");
  FP_reduce(&(ecp3.y));
  FP_redc(x, &(ecp3.y));
  BIG_toCon(x,  fp);
  fprintf(fp,"}} ,");

    fprintf(fp, "{{0X");
  FP_reduce(&(ecp4.x));
  FP_redc(x, &(ecp4.x));
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");
  
  fprintf(fp, "{0x");
  FP_reduce(&(ecp4.y));
  FP_redc(x, &(ecp4.y));
  BIG_toCon(x,  fp);
  fprintf(fp,"}};\n ");

  fprintf(fp, "#endif");
fclose(fp);

}
#else
#include "precomp.h"
int main()
{
    FILE *fp =  fopen("test/precomp/precomp.h", "r");
  if(!fp)
  {
    printf("\tERROR, could not create \"precomp.h\"\n");
    return EXIT_FAILURE;
  }
  BIG x, y;
  ECP ecp1, ecp2, ecp3, ecp4;
  BIG_rcopy(x, m1[0][0]);
  BIG_rcopy(y, m1[0][1]);
  ECP_set(&ecp1, x, y);

  BIG_rcopy(x, m1[0][2]);
  BIG_rcopy(y, m1[0][3]);
  ECP_set(&ecp2, x, y);

  BIG_rcopy(x, m1[1][0]);
  BIG_rcopy(y, m1[1][1]);
  ECP_set(&ecp3, x, y);

  BIG_rcopy(x, m1[1][2]);
  BIG_rcopy(y, m1[1][3]);
  ECP_set(&ecp4, x, y);

  ECP_output(&ecp1); 
  printf("\n");
  ECP_output(&ecp2);
   printf("\n");
  ECP_output(&ecp3);
   printf("\n");
  ECP_output(&ecp4);
   printf("\n");
#endif