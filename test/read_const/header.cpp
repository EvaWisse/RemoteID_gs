

#include "../../include/miracl.h"
void ECP_toHeader(int index, ECP ecp, FILE *fp);
void ECP2_toHeader(int index, ECP2 ecp2, FILE *fp);
void BIG_toCon(BIG x, FILE *fp);

// #define WRITE
#ifdef WRITE
int main()
{
  BIG big, p;
  ECP2 ecp2, P_hat;
  ECP ecp, P;

  ECP_generator(&ecp);
  ECP2_generator(&ecp2);
  BIG_rcopy(p, CURVE_Order);
  ECP_output(&ecp);
  ECP2_output(&ecp2);

  char *raw;
  csprng RNG;
  raw = (char*) malloc(100 * sizeof(char));
  RAND_seed(&RNG, 100, raw);
  free(raw);

  BIG_randomnum(big, p, &RNG);
  
  BIG x, y;
  FILE *fp = fopen("test/read_const/drone_const.h", "w");
  if(!fp)
  {
    printf("\tERROR, could not create \"drone_const.h\"\n");
    return EXIT_FAILURE;
  }
  fprintf(fp, "#include\"../../include/miracl.h\"\n#ifndef CONST_ECP_UAS_H\n#define CONST_ECP_UAS_H\n");
  ECP_toHeader(0, ecp, fp);
  ECP2_toHeader(0, ecp2, fp);
  fprintf(fp, "#endif");
  fclose(fp);
}

#else 
#include "drone_const.h"
int main()
{
  ECP ecp;
  ECP2 ecp2;

  BIG x, y;
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
void ECP_toHeader(int index, ECP ecp, FILE *fp)
{
  BIG x;
  fprintf(fp, "const BIG ECP%d_x= {0x", index);
  FP_reduce(&(ecp.x));
  FP_redc(x, &(ecp.x));
  BIG_toCon(x,  fp);
  fprintf(fp,"};\n");
  
  fprintf(fp, "const BIG ECP%d_y= {0x", index);
  FP_reduce(&(ecp.y));
  FP_redc(x, &(ecp.y));
  BIG_toCon(x,  fp);
  fprintf(fp,"};\n");
}

void ECP2_toHeader(int index, ECP2 ecp2, FILE *fp)
{
  FP2 fp2_x,fp2_y;
  BIG x, y;
  ECP2_get(&fp2_x, &fp2_y, &ecp2);
  FP2_reduce(&fp2_x); FP2_reduce(&fp2_y);
  FP_redc(x, &fp2_x.a);
  FP_redc(y, &fp2_x.b);
  fprintf(fp, "const BIG ECP%d_x1= {0x", index);
  BIG_toCon(x,  fp);
  fprintf(fp,"};\n");

  fprintf(fp, "const BIG ECP%d_y1= {0x", index);
  BIG_toCon(y,  fp);
  fprintf(fp,"};\n");

  FP_redc(x, &fp2_y.a);
  FP_redc(y, &fp2_y.b);
  fprintf(fp, "const BIG ECP%d_x2= {0x", index);
  BIG_toCon(x,  fp);
  fprintf(fp,"};\n");

  fprintf(fp, "const BIG ECP%d_y2= {0x", index);
  BIG_toCon(y,  fp);
  fprintf(fp,"};\n");
}

void BIG_toCon(BIG x, FILE *fp)
{
  int i, len;
  BIG b;
  len = BIG_nbits(x);
  if (len % 4 == 0) len /= 4;
  else
  {
    len /= 4;
    len++;
  }
  if (len < MODBYTES_B256_28 * 2) len = MODBYTES_B256_28 * 2;

  char t[70];
  for(i=0;i<70;i++) t[i]=0;
  for (i = len - 1; i >= 0; i--)
  {
    BIG_copy(b, x);
    BIG_shr(b, i * 4);
    t[i] = (unsigned int) b[0] & 15;
  }

  for(i=0;i<10;i++)
  {
    if(i>0) fprintf(fp, " ,0x");
    for(int j =6;j>=0; j--) fprintf(fp, "%01x", t[j+ (i*7)]);
  }  
}