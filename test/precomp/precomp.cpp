

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
  BIG big[rounds], p;
  ECP2 ecp2, P_hat;
  ECP ecp[rounds], P;

  ECP_generator(&P);
  ECP2_generator(&P_hat);
  BIG_rcopy(p, CURVE_Order);

  char *raw;
  csprng RNG;
  raw = (char*) malloc(100 * sizeof(char));
  RAND_seed(&RNG, 100, raw);
  free(raw);

  for(int i=0;i<rounds;i++)
  {
    BIG_randomnum(big[i], p, &RNG);
    ECP_copy(&ecp[i], &P);
    ECP_mul(&ecp[i], big[i]);
  }
  
  BIG x, y;
  FILE *fp = fopen("test/precomp/precomp.h", "w");
  if(!fp)
  {
    printf("\tERROR, could not create \"drone_const.h\"\n");
    return EXIT_FAILURE;
  }
  fprintf(fp, "#include\"../../include/miracl.h\"\n#ifndef CONST_ECP_UAS_H\n#define CONST_ECP_UAS_H\n");
  fprintf(fp, "const BIG ECP[%d]= {", rounds * 2);
  for(int i=0;i<rounds;i++) ECP_precomp(0, ecp[i], fp);
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};", rounds * 2);

  // ECP2_toHeader(0, ecp2, fp);
  fprintf(fp, "\n#endif");
  fclose(fp);
}

#else 
#include "precomp.h"
int main()
{
  ECP ecp;
  ECP2 ecp2;
  BIG x, y;
  for(int i=0;i<rounds;i++)
  {

  }
  
  BIG_rcopy(x, ECP0_x);
  BIG_rcopy(y, ECP0_y);
  ECP_set(&ecp, x, y);

  FP2 wx, wy;
  FP2_rcopy(&wx, ECP0_x1, ECP0_y1);
  FP2_rcopy(&wy, ECP0_x2, ECP0_y2);
  ECP2_set(&ecp2, &wx, &wy);
  ECP_output(&ecp);
  ECP2_output(&ecp2);
}
#endif
void ECP_precomp(int index, ECP ecp, FILE *fp)
{
  BIG x;
  fprintf(fp, "{0x", index);
  FP_reduce(&(ecp.x));
  FP_redc(x, &(ecp.x));
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");
  
  fprintf(fp, "{0x", index);
  FP_reduce(&(ecp.y));
  FP_redc(x, &(ecp.y));
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");
}

void ECP2_precomp(int index, ECP2 ecp2, FILE *fp)
{
  FP2 fp2_x,fp2_y;
  BIG x, y;
  ECP2_get(&fp2_x, &fp2_y, &ecp2);
  FP2_reduce(&fp2_x); FP2_reduce(&fp2_y);
  FP_redc(x, &fp2_x.a);
  FP_redc(y, &fp2_x.b);
  fprintf(fp, "{0x", index);
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");

  fprintf(fp, "{0x", index);
  BIG_toCon(y,  fp);
  fprintf(fp,"}, ");

  FP_redc(x, &fp2_y.a);
  FP_redc(y, &fp2_y.b);
  fprintf(fp, "{0x", index);
  BIG_toCon(x,  fp);
  fprintf(fp,"}, ");

  fprintf(fp, "const BIG ECP%d_y2= {0x", index);
  BIG_toCon(y,  fp);
  fprintf(fp,"}, ");
}